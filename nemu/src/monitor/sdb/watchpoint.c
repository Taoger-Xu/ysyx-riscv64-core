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

#include "sdb.h"

#define NR_WP 32


static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;
static int wp_count = 0;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].old_val = 0;
    wp_pool[i].expression = NULL;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
/*新增一个监视点*/
WP* new_wp(char *exprseeion, bool *success){
  WP *new_node = free_;
  if(new_node == NULL){
    printf("Watchpoint pool is full\n");
    *success = false;
    return NULL;
  }
  *success = true;
  word_t expr_val = expr(exprseeion, success);
  if(*success == false){
    printf("Error in expr() when exceute new_wp()\n");
  }
  free_ = free_->next;
  // 头插法
  new_node->next = head;
  head = new_node;
  new_node->NO = wp_count ++;
  new_node->old_val = expr_val;
  new_node->expression = (char*)malloc(strlen(exprseeion) + 1);
  if(new_node->expression == NULL){
    panic("Allocate failed in new_node->expression");
  }
  strcpy(new_node->expression, exprseeion);
  *success = true;
  return new_node;
}

/*释放指定监视点malloc的空间*/
int free_wp(){
  return 0;
}

/*检查监视点对应的表达式的值是否变化*/
bool check_wp(){
  WP *ptr = head;
  word_t new_val;
  bool diff = false;
  bool success = true;
  while(ptr){
    new_val = expr(ptr->expression, &success);
    if(success){
      if(new_val != ptr->old_val){
        printf("Watchpoint NO %d triggered: %s\n", ptr->NO, ptr->expression);
        printf("Old value = %lu\n", ptr->old_val);
        printf("New value = %lu\n", new_val);
        ptr->old_val = new_val;
        diff = true;
      }
    }else{
      printf("Watchpoint NO %d : \'%s\' failed to evaluate\n Execution will pause\n", ptr->NO, ptr->expression);
      return true; 
    }
    ptr = ptr->next;
  }
  return diff;
}

/*删除NO.为id的监视点*/
bool delete_wp(int id){
  WP* ptr = head;
  WP* pre = NULL;
  bool found = false;
  while(ptr){
    if(ptr->NO == id){
      found = true;
      break;
    }else{
      pre = ptr;
      ptr = ptr->next;
    }
  }

  if(found){
    free(ptr->expression);
    //删除链表节点
    if(ptr == head){
      head = head->next;
      ptr->next = free_->next;
      free_->next = ptr;
    }else{
      pre->next = ptr->next;
      ptr->next = free_->next;
      free_->next = ptr;
    }
  }
  return found;
}


/*打印的监视点*/
void display_wp(){
  WP *ptr = head;
  if(ptr == NULL){
    printf("No watchpoints now\n");
  }else{
    printf("|\tNO\t|     Old Value      |\t Expr\n");
    while(ptr){
      printf("|\t%d\t|%20ld|  %s\n", ptr->NO, ptr->old_val, ptr->expression);
      ptr = ptr->next;
    }
  }
}