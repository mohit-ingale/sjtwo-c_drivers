#include "cli_handlers.h"

#include "FreeRTOS.h"
#include "sl_string.h"
#include "task.h"

app_cli_status_e a_cli_handler_for_task(app_cli__argument_t argument, sl_string_t user_input_minus_command_name,
                                        app_cli__print_string_function cli_output) {
  sl_string_t s = user_input_minus_command_name;
  cli_output(NULL, s);
  if (sl_string__begins_with_ignore_case(s, "suspend")) {
    sl_string__erase(s, "suspend ");
    cli_output(NULL, s);
    TaskHandle_t task_handle = xTaskGetHandle(s);
    if (NULL == task_handle) {
      sl_string__insert_at(s, 0, "Could not find a task with name:");
      cli_output(NULL, s);
    } else {
      vTaskSuspend(task_handle);
    }

  } else if (sl_string__begins_with_ignore_case(s, "resume")) {
    sl_string__erase(s, "resume ");
    cli_output(NULL, s);
    TaskHandle_t task_handle = xTaskGetHandle(s);
    if (NULL == task_handle) {
      sl_string__insert_at(s, 0, "Could not find a task with name:");
      cli_output(NULL, s);
    } else {
      vTaskResume(task_handle);
    }
  } else {
    cli_output(NULL, "Did you mean to say suspend or resume?\n");
  }

  return APP_CLI_STATUS__SUCCESS;
}
