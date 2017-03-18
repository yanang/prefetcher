CFLAGS = -msse2 --std gnu99 -O0 -Wall -Wextra

EXEC = naive sse sse_prefetch
GIT_HOOKS := .git/hooks/applied

all: $(GIT_HOOKS) $(EXEC)

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

SRCS_common = main.c

naive: $(SRCS_common)
	$(CC) $(CFLAGS) -D NAIVE -o naive main.c

sse: $(SRCS_common)
	$(CC) $(CFLAGS) -D SSE -o sse main.c

sse_prefetch: $(SRCS_common)
	$(CC) $(CFLAGS) -D SSE_PREFETCH -o sse_prefetch main.c

clean:
	$(RM) $(EXEC)
