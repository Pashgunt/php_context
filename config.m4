PHP_ARG_ENABLE(context, whether to enable context support,
[  --enable-context   Enable context support])

if test "$PHP_CONTEXT" != "no"; then
  PHP_NEW_EXTENSION(context, php_context.c context.c, $ext_shared)
fi
