#include <Python.h>
#include <structmember.h>

#if PY_VERSION_HEX < 0x02050000 && !defined(PY_SSIZE_T_MIN)
typedef int Py_ssize_t;
#define PY_SSIZE_T_MAX INT_MAX
#define PY_SSIZE_T_MIN INT_MIN
#endif

typedef struct ObarrObject {
	PyObject_HEAD
	PyObject** pData;
	long	nSize;
	long	nChunkSize;
	long	nAllocSize;
	void*	pInternal_;
} ObarrObject;

#define Obarr_Check(op) PyObject_TypeCheck(op, &ObarrObjectType)

/* internal functions (note lowercase obarr) */
PyObject* obarr_get_element(ObarrObject* self, long index);
long obarr_find(ObarrObject* self, PyObject* other_in);
void obarr_set_element(ObarrObject* self, long index, PyObject* new_in);
void obarr_empty(ObarrObject* self);
void obarr_del_index(ObarrObject* self, long i);
int obarr_valid_index(ObarrObject* self, long i);
int obarr_set_size(ObarrObject* self, long size);

/* exposed API functions (note uppercase Obarr) */
int Obarr_init(ObarrObject *self, PyObject *args, PyObject *kwds);
void Obarr_dealloc(PyObject* self_in);
PyObject* Obarr_repr(PyObject *self_in);
Py_ssize_t Obarr_len(PyObject *self_in);
PyObject * Obarr_item(PyObject *self_in, Py_ssize_t index);
int Obarr_setitem(PyObject* self_in, Py_ssize_t index, PyObject* new_in);
PyObject* Obarr_remove(PyObject* self_in, PyObject* args);
PyObject* Obarr_delete(PyObject* self_in, PyObject* args);
int Obarr_contains(PyObject* self_in, PyObject* other_in);
PyObject* Obarr_index(PyObject* self_in, PyObject* args);
PyObject* Obarr_find(PyObject* self_in, PyObject* args);
PyObject* Obarr_sort(PyObject* self_in, PyObject* args);
PyObject* Obarr_append(PyObject* self_in, PyObject* args);
PyObject* Obarr_resize(PyObject* self_in, PyObject* args);
PyObject* Obarr_clear(PyObject* self_in, PyObject* args);
PyObject* Obarr_debug(PyObject* self_in, PyObject* args);

/* sorting tools */
typedef struct _sortkey
{
	double d;
	long i;
} sortkey;
int compare_doubles(const void *a, const void *b);

extern PySequenceMethods Obarr_as_seq[];
extern PyMethodDef Obarr_methods[];
extern struct PyMemberDef Obarr_members[];
extern PyTypeObject ObarrObjectType;
