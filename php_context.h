#ifndef PHP_CONTEXT_H
#define PHP_CONTEXT_H

extern zend_module_entry context_module_entry;
#define phpext_context_ptr &context_module_entry

#define PHP_CONTEXT_VERSION "0.1"

typedef struct _context_t context_t;

context_t* context_create(time_t timeout);
void context_cancel(context_t *ctx);
zend_bool context_is_cancelled(context_t *ctx);
void context_free(context_t *ctx);
extern zend_module_entry context_module_entry;

#define phpext_context_ptr &context_module_entry

time_t context_get_deadline(context_t *ctx);

#endif
