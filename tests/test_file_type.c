#include "internals.h"
#include <stdio.h>
#include <string.h>

static void null_log(void *ud, int level, const char *msg) { (void)ud;(void)level;(void)msg; }

static const char *file_type_name(file_type_t type) {
  switch (type) {
    case FILE_TYPE_UNKNOWN: return "UNKNOWN";
    case FILE_TYPE_NEC2: return "NEC-2";
    case FILE_TYPE_4NEC2: return "4nec2";
    case FILE_TYPE_NEC3: return "NEC-3";
    case FILE_TYPE_NEC4: return "NEC-4";
    case FILE_TYPE_NEC5: return "NEC-5";
    default: return "INVALID";
  }
}

static int test_file_type(const char *filename, file_type_t expected_type) {
  context_t *ctx = create_context();
  set_log_callback(ctx, null_log, NULL);

  deck_t deck;
  init_deck(&deck);
  FILE *f = fopen(filename, "r");
  if (!f) {
    fprintf(stderr, "Cannot open %s\n", filename);
    return 1;
  }
  read_deck(ctx, &deck, f);
  fclose(f);

  errors_list_t errs = {0};
  parse_deck(ctx, &deck, &errs);

  int result = (deck.file_type == expected_type) ? 0 : 1;
  printf("File: %s\n", filename);
  printf("  Expected: %s (%d)\n", file_type_name(expected_type), expected_type);
  printf("  Got:      %s (%d)\n", file_type_name(deck.file_type), deck.file_type);
  printf("  Result:   %s\n", result == 0 ? "PASS" : "FAIL");
  printf("\n");

  destroy_deck(&deck);
  destroy_context(ctx);
  return result;
}

int main(void) {
  int failures = 0;

  // Create test files
  const char *files[] = {
    "tests/test_nec2.nec",
    "tests/test_4nec2.nec",
    "tests/test_nec3.nec",
    "tests/test_nec4.nec",
    "tests/test_nec4_2.nec",
    "tests/test_nec5.nec",
    "tests/test_unknown.nec",
    "tests/test_sy_cards.nec"
  };

  // Create test NEC-2 file with engine identifier
  FILE *fp = fopen(files[0], "w");
  fprintf(fp, "CM engine: nec-2\n");
  fprintf(fp, "CE\n");
  fprintf(fp, "EX 0\n");
  fprintf(fp, "EN\n");
  fclose(fp);

  // Create test 4nec2 file
  fp = fopen(files[1], "w");
  fprintf(fp, "CM engine: 4nec2\n");
  fprintf(fp, "CE\n");
  fprintf(fp, "EX 0\n");
  fprintf(fp, "EN\n");
  fclose(fp);

  // Create test NEC-3 file
  fp = fopen(files[2], "w");
  fprintf(fp, "CM engine=nec-3\n");
  fprintf(fp, "CE\n");
  fprintf(fp, "EX 0\n");
  fprintf(fp, "EN\n");
  fclose(fp);

  // Create test NEC-4 file
  fp = fopen(files[3], "w");
  fprintf(fp, "CM dialect: nec-4\n");
  fprintf(fp, "CE\n");
  fprintf(fp, "EX 0\n");
  fprintf(fp, "EN\n");
  fclose(fp);

  // Create test NEC-4 file with 4.2 variant
  fp = fopen(files[4], "w");
  fprintf(fp, "CM engine = 4.2\n");
  fprintf(fp, "CE\n");
  fprintf(fp, "EX 0\n");
  fprintf(fp, "EN\n");
  fclose(fp);

  // Create test NEC-5 file
  fp = fopen(files[5], "w");
  fprintf(fp, "CM dialect=nec5\n");
  fprintf(fp, "CE\n");
  fprintf(fp, "EX 0\n");
  fprintf(fp, "EN\n");
  fclose(fp);

  // Create test unknown file (no engine/dialect marker)
  fp = fopen(files[6], "w");
  fprintf(fp, "CM This is a test file\n");
  fprintf(fp, "CE\n");
  fprintf(fp, "EX 0\n");
  fprintf(fp, "EN\n");
  fclose(fp);

  // Create test file with SY cards (should auto-detect as 4nec2)
  fp = fopen(files[7], "w");
  fprintf(fp, "CM Test file with SY cards\n");
  fprintf(fp, "CE\n");
  fprintf(fp, "SY test=value\n");
  fprintf(fp, "EX 0\n");
  fprintf(fp, "EN\n");
  fclose(fp);

  // Test each file type variant
  failures += test_file_type(files[0], FILE_TYPE_NEC2);
  failures += test_file_type(files[1], FILE_TYPE_4NEC2);
  failures += test_file_type(files[2], FILE_TYPE_NEC3);
  failures += test_file_type(files[3], FILE_TYPE_NEC4);
  failures += test_file_type(files[4], FILE_TYPE_NEC4);
  failures += test_file_type(files[5], FILE_TYPE_NEC5);
  failures += test_file_type(files[6], FILE_TYPE_UNKNOWN);
  failures += test_file_type(files[7], FILE_TYPE_4NEC2);

  // Clean up test files
  for (int i = 0; i < 8; i++) {
    remove(files[i]);
  }

  if (failures == 0) {
    printf("All tests passed!\n");
    return 0;
  } else {
    printf("%d test(s) failed!\n", failures);
    return 1;
  }
}


