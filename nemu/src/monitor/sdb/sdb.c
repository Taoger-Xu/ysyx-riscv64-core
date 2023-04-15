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
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include "sdb.h"

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/*sdb需要实现的命令的函数声明*/
static int cmd_help(char *args);
static int cmd_c(char *args);
static int cmd_q(char *args);
static int cmd_si(char *args);
static int cmd_info(char *args);
static int cmd_x(char *args);
static int cmd_p(char *args);
// static int cmd_w(char *args);
// static int cmd_d(char *args);

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si","Step one instruction exactly", cmd_si},
  { "info", "Show integer registers and their contents or status of specified watchpoints", cmd_info},
  {"x", "Examine memory: x N EXPR", cmd_x},
  {"p","Print value of expression EXPR",cmd_p}
  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

/*继续运行被暂停的程序*/
static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

/*退出NEMU*/
static int cmd_q(char *args) {
  return -1;
}

/*打印命令的帮助信息*/
static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

/*让程序单步执行N条指令后暂停执行,当N没有给出时, 缺省为1*/
static int cmd_si(char *args){
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  char* end;
  /*参数为负数*/
  uint64_t n = 0;
  if(arg == NULL){
    n = 1;
  }else{
    if(arg[0] == '-'){
    printf("A syntax error in expression, only one unsigned int value\n");
    return 0;
  }
    n = strtoul(arg, &end, 10);
  }
  /*参数不是整数或者包括非法字符，暂时不做检查*/
  cpu_exec(n);
  return 0;
}
/*打印寄存器状态或者打印监视点信息*/
static int cmd_info(char *args){
  char *arg = strtok(NULL, " ");
  if(arg == NULL){
    printf("List of info subcommands: \n");
    printf("Info r -- List of integer registers and their contents, for selected stack frame.\n");
    printf("Info w -- Status of specified watchpoints\n");
    return 0;
  } 
  if(strcmp(arg, "r") == 0){
    isa_reg_display();
  }else if(strcmp(arg, "w") == 0){
    watchpoint_display();
  }else{
    printf("Undefined info command: %s\n", arg);
  }
  return 0;
}

/*扫描内存,x N EXPR
求出表达式EXPR的值, 将结果作为起始内存地址, 以十六进制形式输出连续的N个4字节*/
static int cmd_x(char *args){
  char *byte_len = strtok(NULL, " ");
  char *expression = strtok(NULL, " ");
  char *end = NULL;
  bool success;
  if(byte_len == NULL){
    printf("Argument required\n");
  }
  else{
    /*参数不是整数或者包括非法字符，暂时不做检查*/
    word_t n = strtoul(byte_len, &end, 10);
    word_t base_addr = expr(expression, &success);
    if(success){
      word_t i;
      word_t addr;
      word_t data;
      for(i = 0; i < n; i ++){
        addr = base_addr + i * 4;
        data = vaddr_read(addr, 4);
        printf("0x%016lX : 0x%08lX\n", addr, data);
      }
    }else{
      printf("A syntax error in expression\n");
    }
  }
  return 0;
}

/*求出表达式EXPR的值*/
static int cmd_p(char *args){
  bool success;
  word_t EXPR;
  EXPR = expr(args, &success);
  if(success){
    printf("the expression result is:%ld\n",EXPR);
  }else{
    printf("Error in the expression,please check it\n");
  }
  return 0;
}
void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
