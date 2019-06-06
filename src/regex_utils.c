#include "regex_utils.h"

static int _get_int_from_ovec(const unsigned char *text, PCRE2_SIZE *ovector, int min_off, int max_off) {
	
	char buffer[265];

	PCRE2_SPTR _start = text + ovector[min_off];
	
	int digit_len = ovector[max_off] - ovector[min_off];
	
	memcpy(&buffer[0], _start, digit_len);
	
	buffer[digit_len] = '\0';
	
	int result = atoi(&buffer[0]);

	return result;
}

static pcre2_match_data * _regex_match_(const unsigned char *_pattern, const unsigned char *_text, int *rc) {
	pcre2_match_data *result = NULL;

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

		result = pcre2_match_data_create_from_pattern(re, NULL);
		
		*rc = pcre2_match(re, value, PCRE2_ZERO_TERMINATED, 0, 0, result, NULL);
	}
	
	pcre2_code_free((pcre2_code*)re);
	
	return result;
}

bool regex_match(const unsigned char *_pattern, const unsigned char *_text) {
	bool found = false;
	int rc;

	pcre2_match_data *match_data = _regex_match_(_pattern, _text, &rc);

	pcre2_match_data_free(match_data);
	
	found = rc > 0;
	
	return found;
}

bool regex_not_blank(const unsigned char *_text) {
	return regex_match((const unsigned char *)"[\\d\\w]+", _text);
}

bool regex_range_match(const unsigned char *range, const unsigned char *value) {
	bool result = false;

	int rc;

	pcre2_match_data *match_data = _regex_match_("^(\\d+)$|^((\\d+)-(\\d+))$", range, &rc);

	if (rc > 0) {
		
		PCRE2_SIZE *ovector;
		ovector = pcre2_get_ovector_pointer(match_data);

		#if debug > 1
			printf("\"^(\\d+)$|^((\\d+)-(\\d+))$\" matches \"%s\" with %i results\n", range,rc);
			printf("Match succeeded at offset %d\n", (int)ovector[0]);
			for (int i = 0; i < rc; i++)
			{
				PCRE2_SPTR substring_start = range + ovector[2*i];
				size_t substring_length = ovector[2*i+1] - ovector[2*i];
				printf("%2d: %.*s\n", i, (int)substring_length, (char *)substring_start);
			}
		#endif

		int min = -1;
		int max = -1;

		switch(rc) {
			case 5: {
				min = _get_int_from_ovec(range, ovector, 6, 7);
				max = _get_int_from_ovec(range, ovector, 8, 9);
				break;
			}
			case 2: {
    			min = _get_int_from_ovec(range, ovector, 2, 3);
				break;
			}
		}

		int lvalue = atoi((char *)value);

		#if debug > 1
			printf("min %i max %i value %i\n", min, max, lvalue);
		#endif

		if(max == -1) {
			result = (lvalue == min);
		} else {
			result = ((lvalue >= min) && (lvalue <= max));
		}

	}

	pcre2_match_data_free(match_data);

	return result;
}