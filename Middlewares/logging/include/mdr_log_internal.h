#ifndef __MDR_LOG_INTERNAL_H__
#define __MDR_LOG_INTERNAL_H__

//these functions do not check level versus MDT_LOCAL_LEVEL, this should be done in stm_log.h
void mdr_log_buffer_hex_internal(const char *tag, const void *buffer, uint16_t buff_len, mdr_log_level_t level);
void mdr_log_buffer_char_internal(const char *tag, const void *buffer, uint16_t buff_len, mdr_log_level_t level);
void mdr_log_buffer_hexdump_internal(const char *tag, const void *buffer, uint16_t buff_len, mdr_log_level_t log_level);

#endif // __MDR_LOG_INTERNAL_H__
