/* Invoking init_serial_stdio() permits using printf() and
   getchar() (at least) from the AVR library, enabling C
   standard interactions. */

/* written by nspring for cmsc216/h Jan 2017 */

/* allows inclusion by C++ code. */
#ifdef __cplusplus
extern "C" {
#endif

/* call this in setup() */
void init_serial_stdio(void);

#ifdef __cplusplus
} /* end extern "C" */
#endif
