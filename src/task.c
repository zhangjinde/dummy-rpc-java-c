#include <string.h>
#include "task.h"
#include "serializer.h"
#include "net.h"

struct task_t cast_task(struct inst instance) {
  struct task_t task = new_task();
  // TODO 本来ならここでserialVersionUIDの比較を行う
  struct field_t *field = instance.u.object.clazz.field;
  struct classdata_t *classdata = instance.u.object.classdata;
  // field と classdata の順番は対応している
  while (field != NULL && classdata != NULL) {
    // field名を見て型を信じる
    if (strcmp("v", field->name) == 0) {
      task.v = classdata->i;
    } else if (strcmp("x", field->name) == 0) {
      task.x = classdata->b;
    } else if (strcmp("str1", field->name) == 0) {
      task.str1 = classdata->obj->u.str;
    } else if (strcmp("str2", field->name) == 0) {
      task.str2 = classdata->obj->u.str;
    }
    field = field->next;
    classdata = classdata->next;
  }
  return task;
};

struct task_t new_task() {
  struct task_t task;
  task.hello = task_hello;
  return task;
}

void task_hello(const struct task_t *self, struct person_t person) {
  struct inst instance = inter_serialize_person(person);
  struct bytes_t bytes = serialize(instance);
  net_call("Task", "hello", bytes);
}

void preview_task(const struct task_t task) {
  printf("task.v: 0x%x\n", task.v);
  printf("task.x: %d\n", task.x);
  printf("task.str1: %s\n", task.str1);
  printf("task.str2: %s\n", task.str2);
}
