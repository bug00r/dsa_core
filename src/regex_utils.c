#include "regex_utils.h"

bool regex_match(const char *_pattern, const char *_text) {
	bool found = false;

	PCRE2_SIZE erroffset;
	int errorcode;
	
	PCRE2_SPTR pattern = (PCRE2_SPTR)_pattern;
	pcre2_code *re = pcre2_compile(pattern, -1, 0, &errorcode, &erroffset, NULL);
	
	if (re == NULL) {
		PCRE2_UCHAR buffer[120];
		(void)pcre2_get_error_message(errorcode, buffer, 120);
		
		#if debug > 0
			printf("pcre2 compile error: %s\n", buffer);
		#endif

	} else {
		int rc;
		PCRE2_SPTR value = (PCRE2_SPTR)_text;
		pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re, NULL);
		rc = pcre2_match(re, value, -1, 0, 0, match_data, NULL);
		pcre2_match_data_free(match_data);
		found = rc > 0;
	}
	
	pcre2_code_free(re);
	
	return found;
}