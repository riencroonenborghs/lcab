#ifndef FUNCH
#define FUNCH
struct sllitem {
	void *		data;
	struct sllitem *next;
};

struct sllitem *sllitem_init(struct sllitem *item, void *data);
struct sllitem *append(struct sllitem *accumulator, struct sllitem *addend);
struct sllitem *prepend(struct sllitem *accumulator, struct sllitem *addend);

int reducer_count(int, struct sllitem *);

int reduce(int initial, struct sllitem *values, int fn(int acc, struct sllitem *value));
/**
 * callback should return 0 for ok # otherwise, this function will
 * return the integer value returned from the first non-zero callback
 */
int for_each(struct sllitem *start, int fn(struct sllitem *, void *ctx), void *ctx);
#endif
