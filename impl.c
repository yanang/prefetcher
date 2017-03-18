#ifndef TRANSPOSE_IMPL
#define TRANSPOSE_IMPL
typedef struct object Object;
typedef void (*func_t)(Object *);

struct object {
    int *src;
    int *dst;
    int w;
    int h;
    func_t naive_transpose, sse_transpose, sse_prefetch_transpose;
};

static void naive_transpose_impl(Object *self)
{
    for (int x = 0; x < self->w; x++)
        for (int y = 0; y < self->h; y++)
            *(self->dst + x * self->h + y) = *(self->src + y * self->w + x);
}

static void sse_transpose_impl(Object *self)
{
    for (int x = 0; x < self->w; x += 4) {
        for (int y = 0; y < self->h; y += 4) {
            __m128i I0 = _mm_loadu_si128((__m128i *)(self->src + (y + 0) * self->w + x));
            __m128i I1 = _mm_loadu_si128((__m128i *)(self->src + (y + 1) * self->w + x));
            __m128i I2 = _mm_loadu_si128((__m128i *)(self->src + (y + 2) * self->w + x));
            __m128i I3 = _mm_loadu_si128((__m128i *)(self->src + (y + 3) * self->w + x));
            __m128i T0 = _mm_unpacklo_epi32(I0, I1);
            __m128i T1 = _mm_unpacklo_epi32(I2, I3);
            __m128i T2 = _mm_unpackhi_epi32(I0, I1);
            __m128i T3 = _mm_unpackhi_epi32(I2, I3);
            I0 = _mm_unpacklo_epi64(T0, T1);
            I1 = _mm_unpackhi_epi64(T0, T1);
            I2 = _mm_unpacklo_epi64(T2, T3);
            I3 = _mm_unpackhi_epi64(T2, T3);
            _mm_storeu_si128((__m128i *)(self->dst + ((x + 0) * self->h) + y), I0);
            _mm_storeu_si128((__m128i *)(self->dst + ((x + 1) * self->h) + y), I1);
            _mm_storeu_si128((__m128i *)(self->dst + ((x + 2) * self->h) + y), I2);
            _mm_storeu_si128((__m128i *)(self->dst + ((x + 3) * self->h) + y), I3);
        }
    }
}

static void sse_prefetch_transpose_impl(Object *self)
{
    for (int x = 0; x < self->w; x += 4) {
        for (int y = 0; y < self->h; y += 4) {
#define PFDIST  8
            _mm_prefetch(self->src+(y + PFDIST + 0) *self->w + x, _MM_HINT_T1);
            _mm_prefetch(self->src+(y + PFDIST + 1) *self->w + x, _MM_HINT_T1);
            _mm_prefetch(self->src+(y + PFDIST + 2) *self->w + x, _MM_HINT_T1);
            _mm_prefetch(self->src+(y + PFDIST + 3) *self->w + x, _MM_HINT_T1);

            __m128i I0 = _mm_loadu_si128 ((__m128i *)(self->src + (y + 0) * self->w + x));
            __m128i I1 = _mm_loadu_si128 ((__m128i *)(self->src + (y + 1) * self->w + x));
            __m128i I2 = _mm_loadu_si128 ((__m128i *)(self->src + (y + 2) * self->w + x));
            __m128i I3 = _mm_loadu_si128 ((__m128i *)(self->src + (y + 3) * self->w + x));
            __m128i T0 = _mm_unpacklo_epi32(I0, I1);
            __m128i T1 = _mm_unpacklo_epi32(I2, I3);
            __m128i T2 = _mm_unpackhi_epi32(I0, I1);
            __m128i T3 = _mm_unpackhi_epi32(I2, I3);
            I0 = _mm_unpacklo_epi64(T0, T1);
            I1 = _mm_unpackhi_epi64(T0, T1);
            I2 = _mm_unpacklo_epi64(T2, T3);
            I3 = _mm_unpackhi_epi64(T2, T3);
            _mm_storeu_si128((__m128i *)(self->dst + ((x + 0) * self->h) + y), I0);
            _mm_storeu_si128((__m128i *)(self->dst + ((x + 1) * self->h) + y), I1);
            _mm_storeu_si128((__m128i *)(self->dst + ((x + 2) * self->h) + y), I2);
            _mm_storeu_si128((__m128i *)(self->dst + ((x + 3) * self->h) + y), I3);
        }
    }
}
#endif /* TRANSPOSE_IMPL */

int init_object(Object **self)
{
    if (NULL == (*self = malloc(sizeof(Object)))) return -1;
    (*self)->src = 0;
    (*self)->dst = 0;
    (*self)->w = 0;
    (*self)->h = 0;
    (*self)->naive_transpose = naive_transpose_impl;
    (*self)->sse_transpose = sse_transpose_impl;
    (*self)->sse_prefetch_transpose = sse_prefetch_transpose_impl;
    return 0;
}
