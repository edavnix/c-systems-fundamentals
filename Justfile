# Justfile for c-systems-fundamentals
# Documentation: https://just.systems/man/en/

set shell := ["bash", "-euo", "pipefail", "-c"]

# Compiler and Flags
CC        := "gcc"
CFLAGS    := "-Wall -Wextra -std=c99 -pthread"
TARGET    := "exercise"

# Directories
SRC_DIR   := "src"
FILES_DIR := "files"

# Colors using ANSI escape codes
RED       := '\033[0;31m'
GREEN     := '\033[0;32m'
YELLOW    := '\033[0;33m'
CYAN      := '\033[0;36m'
GRAY      := '\033[0;90m'
END       := '\033[0m'

# Status Prefixes
ERROR     := RED    + "ERROR  " + END
INFO      := YELLOW + "INFO   " + END
SUCCESS   := GREEN  + "SUCCESS" + END
EXEC      := CYAN   + "EXEC   " + END

default: help

# Run a specific exercise. Usage: just run <file>
[no-exit-message]
run file_input:
  #!/usr/bin/env bash
  filename="{{file_input}}"
  [[ "$filename" != *.c ]] && filename="${filename}.c"

  # Search in current dir or src/ (including subdirs)
  if [[ -f "$filename" ]]; then
    target="$filename"
  elif [[ -f "{{SRC_DIR}}/$filename" ]]; then
    target="{{SRC_DIR}}/$filename"
  else
    target=$(find {{SRC_DIR}} -type f -name "$filename" \
             -print -quit)
    [[ -z "$target" ]] && {
      echo -e "{{ERROR}} File '$filename' not found."
      exit 1
    }
  fi

  # Create output directory for file operations
  mkdir -p {{FILES_DIR}}

  # Compile
  if ! {{CC}} {{CFLAGS}} "$target" -o {{TARGET}} 2>&1; then
    echo -e "{{ERROR}} Compilation failed for '$target'"
    exit 1
  fi

  # Execute
  echo -e "{{EXEC}} Executing '$target'"
  ./{{TARGET}} || true
  echo -e "{{SUCCESS}} Execution finished"

  # Cleanup
  rm -f {{TARGET}} {{FILES_DIR}}/*.bin

# List exercises by module
[no-exit-message]
list:
  #!/usr/bin/env bash
  [[ ! -d "{{SRC_DIR}}" ]] && {
    echo -e "{{ERROR}} Dir '{{SRC_DIR}}' not found."
    exit 1
  }

  modules=$(find {{SRC_DIR}} -mindepth 1 -maxdepth 1 -type d | sort)

  [[ -z "$modules" ]] && {
    echo -e "{{ERROR}} No modules in '{{SRC_DIR}}'"
    exit 1
  }

  echo -e "{{INFO}} Available exercises by module:"

  while IFS= read -r dir; do
    module=$(basename "$dir")
    echo -e "  {{CYAN}}$module/{{END}}"

    exercises=$(find "$dir" -maxdepth 1 -type f -name "*.c" | sort)

    if [[ -z "$exercises" ]]; then
      echo -e "    {{GRAY}}(empty){{END}}"
      continue
    fi

    echo "$exercises" | xargs -n 1 basename | \
      sed 's/^/    /' | column
  done <<< "$modules"

# Check syntax
[no-exit-message]
check:
  #!/usr/bin/env bash
  echo -e "{{INFO}} Checking syntax..."

  # Check if directory exists
  [[ ! -d "{{SRC_DIR}}" ]] && {
    echo -e "{{ERROR}} Dir '{{SRC_DIR}}' not found."
    exit 1
  }

  passed=0 failed=0 found_files=0

  while IFS= read -r file; do
    [[ -e "$file" ]] || continue
    found_files=1

    if {{CC}} {{CFLAGS}} -fsyntax-only "$file" \
       &>/dev/null; then
      ((passed++))
    else
      echo -e "{{RED}}  -> FAIL {{END}} $file"
      ((failed++))
    fi
  done < <(find {{SRC_DIR}} -type f -name "*.c")

  [[ $found_files -eq 0 ]] && {
    echo -e "{{ERROR}} No files in {{SRC_DIR}}/"
    exit 1
  }

  echo -e "{{INFO}}" \
          "{{GREEN}}$passed passed {{END}},{{RED}} $failed failed {{END}}"

  [[ $failed -ne 0 ]] && exit 1

# Show help
help:
  @echo -e "{{INFO}} Usage:"
  @echo -e "  just run <file>   {{CYAN}}# Run specific file {{END}}"
  @echo -e "  just list         {{CYAN}}# List all exercises {{END}}"
  @echo -e "  just check        {{CYAN}}# Check syntax for all files {{END}}"
