/*
 ===============================================================================
 Exercise: 02_unit_converter.c
 Description: Unit converter with interactive menu
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Temperature conversion (Celsius, Fahrenheit, Kelvin)
 - Length conversion (meters, kilometers, miles, feet)
 - Weight conversion (kilograms, pounds, ounces)
 - Time conversion (seconds, minutes, hours)
 - Input validation and error handling
 - Case-insensitive unit input
 ===============================================================================
*/

#include <stdio.h>

#define TRUE 1
#define MIN_OPTION 1
#define MAX_OPTION 5

#define KELVIN_OFFSET 273.15
#define FAHRENHEIT_RATIO 1.8
#define FAHRENHEIT_OFFSET 32.0

#define METERS_PER_KM 1000.0
#define METERS_PER_MILE 1609.34
#define METERS_PER_FOOT 0.3048

#define KG_PER_POUND 0.453592
#define KG_PER_OUNCE 0.0283495

#define SECONDS_PER_MINUTE 60.0
#define SECONDS_PER_HOUR 3600.0

typedef enum {
  SUCCESS,
  ERR_INVALID_UNIT,
  ERR_INVALID_OPTION,
  ERR_INVALID_INPUT
} Status;

typedef struct {
  Status status;
  double value;
} Result;

void show_menu(void);
void show_unit_options(int option);
void handle_error(Status status);
void run_temperature_conversion(void);
void run_length_conversion(void);
void run_weight_conversion(void);
void run_time_conversion(void);

void clear_input_buffer(void);
Status read_integer(int *value);
Status read_double(double *value);
Status read_char(char *value);
Status read_conversion_input(double *value, char *from, char *to);

Result convert_temperature(double value, char from, char to);
Result convert_length(double value, char from, char to);
Result convert_weight(double value, char from, char to);
Result convert_time(double value, char from, char to);

int main(void) {
  int option = 0;

  while (TRUE) {
    show_menu();

    if (read_integer(&option) != SUCCESS) {
      handle_error(ERR_INVALID_INPUT);
      continue;
    }

    if (option == MAX_OPTION) {
      printf("\nThank you for using the converter!\n");
      break;
    }

    if (option < MIN_OPTION || option > MAX_OPTION) {
      handle_error(ERR_INVALID_OPTION);
      continue;
    }

    show_unit_options(option);

    switch (option) {
    case 1:
      run_temperature_conversion();
      break;
    case 2:
      run_length_conversion();
      break;
    case 3:
      run_weight_conversion();
      break;
    case 4:
      run_time_conversion();
      break;
    }
  }

  return 0;
}

void show_menu(void) {
  printf("===== Unit Converter =====\n\n");
  printf("1. Temperature\n2. Length\n3. Weight\n4. Time\n5. Exit\n");
  printf("Select an option: ");
}

void show_unit_options(int option) {
  switch (option) {
  case 1:
    printf("\nUnits: C (Celsius), F (Fahrenheit), K (Kelvin)\n");
    break;
  case 2:
    printf("\nUnits: M (meters), K (km), I (miles), F (feet)\n");
    break;
  case 3:
    printf("\nUnits: K (kg), P (pounds), O (ounces)\n");
    break;
  case 4:
    printf("\nUnits: S (seconds), M (minutes), H (hours)\n");
    break;
  }
}

void handle_error(Status status) {
  switch (status) {
  case ERR_INVALID_UNIT:
    printf("Error: Invalid unit selected.\n\n");
    break;
  case ERR_INVALID_OPTION:
    printf("Error: Invalid option.\n\n");
    break;
  case ERR_INVALID_INPUT:
    printf("Error: Invalid input.\n\n");
    break;
  case SUCCESS:
    break;
  }
}

void run_temperature_conversion(void) {
  double value;
  char from, to;

  Status status = read_conversion_input(&value, &from, &to);
  if (status != SUCCESS) {
    handle_error(status);
    return;
  }

  Result res = convert_temperature(value, from, to);
  if (res.status == SUCCESS) {
    printf("\n  - Result: %.2f %c\n\n", res.value, to);
  } else {
    handle_error(res.status);
  }
}

void run_length_conversion(void) {
  double value;
  char from, to;

  Status status = read_conversion_input(&value, &from, &to);
  if (status != SUCCESS) {
    handle_error(status);
    return;
  }

  Result res = convert_length(value, from, to);
  if (res.status == SUCCESS) {
    printf("\n  - Result: %.2f %c\n\n", res.value, to);
  } else {
    handle_error(res.status);
  }
}

void run_weight_conversion(void) {
  double value;
  char from, to;

  Status status = read_conversion_input(&value, &from, &to);
  if (status != SUCCESS) {
    handle_error(status);
    return;
  }

  Result res = convert_weight(value, from, to);
  if (res.status == SUCCESS) {
    printf("\n  - Result: %.2f %c\n\n", res.value, to);
  } else {
    handle_error(res.status);
  }
}

void run_time_conversion(void) {
  double value;
  char from, to;

  Status status = read_conversion_input(&value, &from, &to);
  if (status != SUCCESS) {
    handle_error(status);
    return;
  }

  Result res = convert_time(value, from, to);
  if (res.status == SUCCESS) {
    printf("\n  - Result: %.2f %c\n\n", res.value, to);
  } else {
    handle_error(res.status);
  }
}

void clear_input_buffer(void) {
  int c;
  while ((c = getchar()) != '\n' && c != EOF) {
    ;
  }
}

Status read_integer(int *value) {
  if (scanf("%d", value) != 1) {
    clear_input_buffer();
    return ERR_INVALID_INPUT;
  }
  clear_input_buffer();
  return SUCCESS;
}

Status read_double(double *value) {
  if (scanf("%lf", value) != 1) {
    clear_input_buffer();
    return ERR_INVALID_INPUT;
  }
  clear_input_buffer();
  return SUCCESS;
}

Status read_char(char *value) {
  if (scanf(" %c", value) != 1) {
    clear_input_buffer();
    return ERR_INVALID_INPUT;
  }
  clear_input_buffer();
  return SUCCESS;
}

Status read_conversion_input(double *value, char *from, char *to) {
  printf("Enter value: ");
  if (read_double(value) != SUCCESS)
    return ERR_INVALID_INPUT;

  printf("Enter source unit: ");
  if (read_char(from) != SUCCESS)
    return ERR_INVALID_INPUT;

  printf("Enter target unit: ");
  if (read_char(to) != SUCCESS)
    return ERR_INVALID_INPUT;

  return SUCCESS;
}

Result convert_temperature(double value, char from, char to) {
  Result res = {SUCCESS, 0.0};
  double celsius;

  if (from == 'C' || from == 'c') {
    celsius = value;
  } else if (from == 'F' || from == 'f') {
    celsius = (value - FAHRENHEIT_OFFSET) / FAHRENHEIT_RATIO;
  } else if (from == 'K' || from == 'k') {
    celsius = value - KELVIN_OFFSET;
  } else {
    res.status = ERR_INVALID_UNIT;
    return res;
  }

  if (to == 'C' || to == 'c') {
    res.value = celsius;
  } else if (to == 'F' || to == 'f') {
    res.value = celsius * FAHRENHEIT_RATIO + FAHRENHEIT_OFFSET;
  } else if (to == 'K' || to == 'k') {
    res.value = celsius + KELVIN_OFFSET;
  } else {
    res.status = ERR_INVALID_UNIT;
  }

  return res;
}

Result convert_length(double value, char from, char to) {
  Result res = {SUCCESS, 0.0};
  double meters;

  if (from == 'M' || from == 'm') {
    meters = value;
  } else if (from == 'K' || from == 'k') {
    meters = value * METERS_PER_KM;
  } else if (from == 'I' || from == 'i') {
    meters = value * METERS_PER_MILE;
  } else if (from == 'F' || from == 'f') {
    meters = value * METERS_PER_FOOT;
  } else {
    res.status = ERR_INVALID_UNIT;
    return res;
  }

  if (to == 'M' || to == 'm') {
    res.value = meters;
  } else if (to == 'K' || to == 'k') {
    res.value = meters / METERS_PER_KM;
  } else if (to == 'I' || to == 'i') {
    res.value = meters / METERS_PER_MILE;
  } else if (to == 'F' || to == 'f') {
    res.value = meters / METERS_PER_FOOT;
  } else {
    res.status = ERR_INVALID_UNIT;
  }

  return res;
}

Result convert_weight(double value, char from, char to) {
  Result res = {SUCCESS, 0.0};
  double kg;

  if (from == 'K' || from == 'k') {
    kg = value;
  } else if (from == 'P' || from == 'p') {
    kg = value * KG_PER_POUND;
  } else if (from == 'O' || from == 'o') {
    kg = value * KG_PER_OUNCE;
  } else {
    res.status = ERR_INVALID_UNIT;
    return res;
  }

  if (to == 'K' || to == 'k') {
    res.value = kg;
  } else if (to == 'P' || to == 'p') {
    res.value = kg / KG_PER_POUND;
  } else if (to == 'O' || to == 'o') {
    res.value = kg / KG_PER_OUNCE;
  } else {
    res.status = ERR_INVALID_UNIT;
  }

  return res;
}

Result convert_time(double value, char from, char to) {
  Result res = {SUCCESS, 0.0};
  double seconds;

  if (from == 'S' || from == 's') {
    seconds = value;
  } else if (from == 'M' || from == 'm') {
    seconds = value * SECONDS_PER_MINUTE;
  } else if (from == 'H' || from == 'h') {
    seconds = value * SECONDS_PER_HOUR;
  } else {
    res.status = ERR_INVALID_UNIT;
    return res;
  }

  if (to == 'S' || to == 's') {
    res.value = seconds;
  } else if (to == 'M' || to == 'm') {
    res.value = seconds / SECONDS_PER_MINUTE;
  } else if (to == 'H' || to == 'h') {
    res.value = seconds / SECONDS_PER_HOUR;
  } else {
    res.status = ERR_INVALID_UNIT;
  }

  return res;
}
