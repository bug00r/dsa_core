#include "regex_utils.h"

bool regex_match(const unsigned char *_pattern, const unsigned char *_text) {
	bool found = false;

	PCRE2_SIZE erroffset;
	int errorcode;
	
	PCRE2_SPTR pattern = (PCRE2_SPTR)_pattern;
	const pcre2_code *re = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, 0, &errorcode, &erroffset, NULL);
	
	if (re == NULL) {
		PCRE2_UCHAR buffer[120];
		(void)pcre2_get_error_message(errorcode, buffer, sizeof(buffer));
		
		#if debug > 0
			printf("pcre2 compile error: %s\n", buffer);
		#endif

	} else if (re != NULL ) {
		
		PCRE2_SPTR value = (PCRE2_SPTR)_text;
		//PCRE2_SIZE len = strlen((char *)value);
		pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re, NULL);
		int rc = pcre2_match(re, value, PCRE2_ZERO_TERMINATED, 0, 0, match_data, NULL);
		pcre2_match_data_free(match_data);
		found = rc > 0;
	}
	
	pcre2_code_free((pcre2_code*)re);
	
	return found;
}

bool regex_not_blank(const unsigned char *_text) {
	return regex_match((const unsigned char *)"[\\d\\w]+", _text);
}