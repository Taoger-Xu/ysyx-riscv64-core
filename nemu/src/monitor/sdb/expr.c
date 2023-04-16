/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include "sdb.h"
enum {
  TK_NOTYPE = 256, //空格
  TK_EQ,           //==
  /* TODO: Add more token types */
  TK_LBRACKET,     //括号
  TK_RBRACKET,

  TK_HEX,          //操作数
  TK_DEC,
  TK_REG,
                  //运算符
  TK_PLUS,
  TK_SUB_MINUS,  //减号或者负号
  TK_SUB,
  TK_MINUS,
  TK_MUL_DEREF,  //乘号或者解引用
  TK_MUL,
  TK_DEREF,
  TK_DIV,
  TK_MOD,        // %
  TK_NEQ,        // !=
  TK_NOT,        // !
  TK_LE,
  TK_LT,
  TK_GE,
  TK_GT,
  TK_AND,
  TK_OR
};
#define IS_OPERAND(x) (x == TK_DEC || x == TK_HEX || x == TK_REG)
#define IS_SINGLE_OPERATOR(x) (x == TK_DEREF || x == TK_MINUS || x == TK_NOT)
#define MEM_READ_LEN 8
static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  {" +", TK_NOTYPE},    // spaces,匹配多个空格
  {"\\+", TK_PLUS},     // plus
  {"==", TK_EQ},        // equal
  {"-",  TK_SUB_MINUS}, //sub or minus
  {"\\*",TK_MUL_DEREF}, //mul or deref
  {"/",  TK_DIV},       //div
  {"%",  TK_MOD},
  {"0[xX][0-9a-fA-F]+",TK_HEX},  //十六进制整数优先匹配于十进制整数
  {"[0-9]+", TK_DEC},   //十进制整数
  {"^(\\$)[0-9a-z]+",TK_REG},  //reg,寄存器
  {"\\(", TK_LBRACKET},  //左括号
  {"\\)", TK_RBRACKET},  //右括号
  {"&&", TK_AND},        //与运算,必须放在&前面,regex优先匹配
  {"\\|\\|", TK_OR},     //或运算
  {"!=", TK_NEQ},        //不等于
  {"!",  TK_NOT},        //取反运算
  {"<=", TK_LE},         // <=
  {"<",  TK_LT},         // <
  {">=", TK_GE},         // >=
  {">",  TK_GT}          // >
};

#define NR_REGEX ARRLEN(rules)
#define NR_TOKENS  100  //tokens数组的最大长度 
static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char *str; // 动态分配,防止token过长造成缓冲区溢出
} Token;

static Token tokens[NR_TOKENS] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE: break;
          default: 
               tokens[nr_token].type = rules[i].token_type;
               tokens[nr_token].str = (char *)malloc((substr_len + 1) * sizeof(char));
               strncpy(tokens[nr_token].str, substr_start, substr_len);
               tokens[nr_token].str[substr_len] = '\0';
               nr_token++;
               break;
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

static word_t strtodec(char *str){
    int i = 0;
    word_t res = 0;
    while(str[i] != '\0'){
      res = res * 10 + str[i] - '0';
      i+=1;
    }
    return res;
}

static word_t strtohex(char *str){
  int i = 2;
  word_t res = 0;
  while(str[i] != '\0'){
    if(str[i] >= '0' && str[i] <= '9'){
      res = res * 16 + str[i] - '0';
    }else{
      res = res * 16 + str[i] - 'a' + 10;
    }
    i+=1;
  }
  return res;
}

/*检测一个表达式是否被一对括号包裹着以及中间表达式括号是否匹配*/
static bool check_parentheses(int l, int r){
  //检测表达式是否合法,非法的停止运算
  int cnt = 0;
  //两端必须为左右括号, 主要排除 a + b
  bool flag1 = tokens[l].type == TK_LBRACKET;
  bool flag2 = tokens[r].type == TK_RBRACKET;
  if(flag1 == false || flag2 == false) return false;

  //去掉两端后中间左右括号数量相等，且扫描过程中(数量不小于), 排除 (a) + (b)
  for(int i = l + 1; i < r; i ++){
    if(tokens[i].type == TK_LBRACKET){ // 进栈
        cnt++;
    }else if(tokens[i].type == TK_RBRACKET){
      if(cnt == 0){
        return false;
      }
      cnt--;
    }
  }
  return true;
}

/*得到运算符的优先级,数值越低优先级越高,参考C Operator Precedence*/
static int get_prior(int op){
    switch (op){
    case TK_LBRACKET : case TK_RBRACKET:
          return 1;
    //单目运算符
    case TK_DEREF : case TK_MINUS : case TK_NOT:
          return 3;
    case TK_MUL: case TK_DIV: case TK_MOD:
          return 5;
    case TK_PLUS: case TK_SUB:
          return 6;
    case TK_LE: case TK_LT : case TK_GE : case TK_GT:
          return 9;
    case TK_EQ: case TK_NEQ:
          return 10;
    case TK_AND:
          return 14;
    case TK_OR:
          return 15;
    default:
          panic("Op: %d is not existed", op);
          break;
    }
}


/* l, r 分别是子表达式的开始位置和结束位置,succeed用来传递求值函数是否成功*/
word_t eval(int l, int r, bool *succeed){

  word_t res;
  int main_op_pos = -1; //主运算符位置.
  int bracket_num = 0; // 左括号数量，判断表达式括号是否匹配
  int op_prior;    // 主运算符的优先级最低
  int low_prior = -1; //最高优先级

  if(l > r){
    *succeed = false;
    printf("Bad expression, eval failed in %d\n", l);
    return 0;
  }
  else if( l == r){
    /* Single token.
     * For now this token should be a number. Return the value of the number.
     */
    if(tokens[l].type == TK_DEC){ //十进制
      *succeed = true;
      return strtodec(tokens[l].str);
    }
    else if(tokens[l].type == TK_HEX){ //十六进制
      *succeed = true;
      return strtohex(tokens[l].str);
    } 
    else if(tokens[l].type == TK_REG){ //寄存器
      char *token_str = tokens[l].str + 1;
      res = isa_reg_str2val(token_str, succeed);
      if(*succeed == false){
        printf("Bad expression, register %s is not existed\n", token_str);
        return 0;
      }
      return res;
    }
    else{
      printf("Bad expression, token is not a number or register\n");
      *succeed = false;
      return 0;
    }
  }
  else if(check_parentheses(l, r) == true){
    return eval(l + 1, r - 1, succeed);
  }
  else{
    /*寻找主运算符*/
    for(int i = l; i <= r; i++){
        switch (tokens[i].type){
        //非运算符的token不是主运算符
        case TK_DEC : case TK_HEX : case TK_REG :
          break;
        case TK_LBRACKET:
          bracket_num++;
          break;
        case TK_RBRACKET:
          if(bracket_num == 0){
            //括号不合法终止计算
            *succeed = false;
            panic("Bad expression, the parentheses are not legal");
            }
          bracket_num--;
          break;
        // 单目运算符，即主运算符为单目运算符时pos=-1
        case TK_DEREF : case TK_NOT:  case TK_MINUS:
          break;
        default:
          //bracket_num == 0判断出现在一对括号中的token不是主运算符
          //主运算符的优先级在表达式中是最低的，最后被结合的运算符才是主运算符
          op_prior = get_prior(tokens[i].type);
          if((bracket_num == 0) && op_prior >= low_prior){
              low_prior = op_prior;
              main_op_pos = i;
          }
          break;
        }
    }
    if(bracket_num != 0){
      //括号不合法终止计算
      *succeed = false;
      panic("Bad expression, the parentheses are not legal");
    }

    //主运算符为单目运算符
    if(main_op_pos == -1){
        switch (tokens[l].type){
        case TK_NOT:
          return !eval(l+1, r, succeed);
        case TK_DEREF:
          res = eval(l+1, r, succeed);
          if(*succeed == false){
            printf("Memory read in address 0x%016lX wrong\n", res);
            return 0;
          }
          return vaddr_read(res, MEM_READ_LEN);
        case TK_MINUS:
          return -eval(l+1, r, succeed); 
        default:
          break;
        }
    }
    // 递归计算
    else{
      word_t eval1 = eval(l, main_op_pos - 1 , succeed);
      word_t eval2 = eval(main_op_pos + 1, r, succeed);
      switch (tokens[main_op_pos].type){
      case TK_PLUS:
        if(*succeed == false){
          printf("Calculate send errors at location %d\n", main_op_pos);
          return 0;
        } 
        return eval1 + eval2;
        break;
      case TK_SUB:
        if(*succeed == false){
          printf("Calculate send errors at location %d\n", main_op_pos);
          return 0;
        } 
        return eval1 - eval2;
        break;
      case TK_MUL:
        if(*succeed == false){
          printf("Calculate send errors at location %d\n", main_op_pos);
          return 0;
        } 
        return eval1 * eval2;
        break;
      case TK_DIV:
        if(*succeed == false){
          printf("Calculate send errors at location %d\n", main_op_pos);
          return 0;
        } 
        return eval1 / eval2;
        break;
      case TK_MOD:
        if(*succeed == false){
          printf("Calculate send errors at location %d\n", main_op_pos);
          return 0;
        } 
        return eval1 % eval2;
        break;
      case TK_EQ:
        if(*succeed == false){
          printf("Calculate send errors at location %d\n", main_op_pos);
          return 0;
        } 
        return eval1 == eval2;
        break;
      case TK_NEQ:
        if(*succeed == false){
          printf("Calculate send errors at location %d\n", main_op_pos);
          return 0;
        } 
        return eval1 != eval2;
        break;  
      case TK_AND:
        if(*succeed == false){
          printf("Calculate send errors at location %d\n", main_op_pos);
          return 0;
        } 
        return eval1 && eval2;
        break; 
      case TK_OR:
        if(*succeed == false){
          printf("Calculate send errors at location %d\n", main_op_pos);
          return 0;
        } 
        return eval1 || eval2;
        break; 
      case TK_LT:
        if(*succeed == false){
          printf("Calculate send errors at location %d\n", main_op_pos);
          return 0;
        } 
        return eval1 < eval2;
        break; 
      case TK_LE:
        if(*succeed == false){
          printf("Calculate send errors at location %d\n", main_op_pos);
          return 0;
        } 
        return eval1 <= eval2;
        break; 
      case TK_GT:
        if(*succeed == false){
          printf("Calculate send errors at location %d\n", main_op_pos);
          return 0;
        } 
        return eval1 > eval2;
        break; 
      case TK_GE:
        if(*succeed == false){
          printf("Calculate send errors at location %d\n", main_op_pos);
          return 0;
        } 
        return eval1 >= eval2;
        break; 
      default:
        panic("No such Operator");
        break;
      }
    }
  }
  return 0;
}

/*将之前make_token中*和-的具体类型识别出来*/
static void analyse_token(){
  for(int i = 0; i < nr_token; i++){
    //是负号的条件: 
    // 1. 第一个字符: -1; 
    // 2. 前一个字符不是操作数
    // 3. 前面一个字符不是),即是表达式
    if(tokens[i].type == TK_SUB_MINUS){
      if(i == 0 || (!IS_OPERAND(tokens[i-1].type) && tokens[i-1].type != TK_RBRACKET)){
          tokens[i].type = TK_MINUS;
      }else{
          tokens[i].type = TK_SUB;
      }
    }
   //是解引用的条件: 同上
    if(tokens[i].type == TK_MUL_DEREF){
      if(i == 0 || (!IS_OPERAND(tokens[i-1].type) && tokens[i-1].type != TK_RBRACKET)){
          tokens[i].type = TK_DEREF;
      }else{
          tokens[i].type = TK_MUL;
      }
    }
  }
}
/*释放之前malloc的内容*/
static void free_token(){
  for(int i = 0; i < nr_token; i++){
    if(tokens[i].str != NULL){
      free(tokens[i].str);
    }
  }
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  /* TODO: Insert codes to evaluate the expression. */
  /*将之前make_token中*和-的具体类型识别出来*/
  analyse_token();
  *success = true;
  word_t res = 0;
  res = eval(0, nr_token - 1, success);
  /*释放之前malloc的内容*/
  free_token();
  return res;
  return 0;
}
