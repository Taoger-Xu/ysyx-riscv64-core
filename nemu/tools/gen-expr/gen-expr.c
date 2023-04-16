#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

#define EXPR_MAX 30
static uint32_t expr_len = 0;
// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned long int result = %s; "
"  printf(\"%%lu\", result); "
"  return 0; "
"}";

static char *pbuf;
static int nr_op = 0;
#define format_buf(fmt, ...) pbuf += sprintf(pbuf, fmt, ##__VA_ARGS__)

static inline uint64_t choose(uint64_t max) {
  return rand() % max;
}

static inline void gen_rand_op() {
  char op_list[] = {'+', '-', '*', '/', '+', '-', '*'};
  format_buf("%c", op_list[choose(7)]);
}

static inline void gen_num() {
  uint64_t num = (uint64_t)rand() % 256;
  if(num == 0) num = 1;
  format_buf("%lulu", num);
}

static inline void gen_space() {
  char *space_list[3] = {
    "",
    " ",
    "  ",
  };
  format_buf("%s", space_list[choose(3)]);
}

static inline void gen_rand_expr() {
  expr_len+=1;
  if(expr_len>EXPR_MAX) return;
  gen_space();
  switch (choose(3)) {
    default:
      if (nr_op == 0) gen_rand_expr();
      else gen_num();
      break;
    case 1:
      format_buf("(");
      gen_rand_expr();
      format_buf(")");
      break;
    case 0:
      nr_op ++;
      if (pbuf - buf >= sizeof(buf) / 2) {
        gen_num();
        break;
      }
      gen_rand_expr();
      gen_space();
      gen_rand_op();
      gen_space();
      gen_rand_expr();
      break;
  }
  gen_space();
}
void remove_u(char *p) {
  char *q = p;
  while ((q = strchr(q, 'u')) != NULL) {
    // reuse code_buf
    strcpy(code_buf, q + 1);
    strcpy(q, code_buf);
  }
}

void remove_l(char *p) {
  char *q = p;
  while ((q = strchr(q, 'l')) != NULL) {
    // reuse code_buf
    strcpy(code_buf, q + 1);
    strcpy(q, code_buf);
  }
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    nr_op = 0;
    expr_len = 0;
    pbuf = buf;
    buf[0]='\0';
    gen_rand_expr();
    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    uint64_t result;
    remove_u(buf);
    remove_l(buf);
    if(fscanf(fp, "%lu", &result))
      printf("%lu %s\n", result, buf);
    else
      printf("error!");
    pclose(fp);
    
  }
  return 0;
}