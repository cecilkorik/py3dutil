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


/* Python object definition structures */
static PySequenceMethods Obarr_as_seq[] = {
	Obarr_len,			/* sq_length */
	0,					/* sq_concat */
	0,					/* sq_repeat */
	Obarr_item,			/* sq_item */
	0,					/* sq_slice */
	Obarr_setitem,		/* sq_ass_item */
	0,					/* sq_ass_slice */
	Obarr_contains,		/* sq_contains */
};

static PyMethodDef Obarr_methods[] = {
	{"resize", (PyCFunction)Obarr_resize, METH_VARARGS, "allocate the array to a new size"},
	{"clear", (PyCFunction)Obarr_clear, METH_NOARGS, "delete everything in the array"},
	{"append", (PyCFunction)Obarr_append, METH_VARARGS, "append the object to the array"},
	{"find", (PyCFunction)Obarr_find, METH_VARARGS, "find the index of an object in the array"},
	{"index", (PyCFunction)Obarr_index, METH_VARARGS, "find the index of an object in the array"},
	{"debug", (PyCFunction)Obarr_debug, METH_NOARGS, "debug"},
	{"delete", (PyCFunction)Obarr_delete, METH_VARARGS, "delete a specific index in the array"},
	{"remove", (PyCFunction)Obarr_remove, METH_VARARGS, "remove an object from the array"},
	{"sort", (PyCFunction)Obarr_sort, METH_VARARGS, "sorts the array using another obarr of floats as sortkeys"},
	{NULL}
};

static struct PyMemberDef Obarr_members[] = {
	/*{"x", T_OBJECT_EX, offsetof(ObarrObject, x), 0, "x"},
	{"y", T_OBJECT_EX, offsetof(ObarrObject, y), 0, "y"},
	{"z", T_OBJECT_EX, offsetof(ObarrObject, z), 0, "z"},*/
	{NULL}  /* Sentinel */
};


static PyTypeObject ObarrObjectType = {
	PyObject_HEAD_INIT(NULL)
	0,				/* ob_size        */
	"pyobarr.obarr",		/* tp_name        */
	sizeof(ObarrObject),		/* tp_basicsize   */
	0,				/* tp_itemsize    */
	Obarr_dealloc,	/* tp_dealloc     */
	0,				/* tp_print       */
	0,				/* tp_getattr     */
	0,				/* tp_setattr     */
	0,				/* tp_compare     */
	Obarr_repr,	    /* tp_repr        */
	0,	/* tp_as_number   */
	Obarr_as_seq,    /* tp_as_sequence */
	0,				/* tp_as_mapping  */
	0,				/* tp_hash        */
	0,				/* tp_call        */
	0,				/* tp_str         */
	0,				/* tp_getattro    */
	0,				/* tp_setattro    */
	0,				/* tp_as_buffer   */
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_CHECKTYPES,		/* tp_flags       */
	"Vector objects are simple.",	/* tp_doc         */
	0,				/* tp_traverse       */
	0,				/* tp_clear          */
	0,				/* tp_richcompare    */
	0,				/* tp_weaklistoffset */
	0,				/* tp_iter           */
	0,				/* tp_iternext       */
	Obarr_methods,   /* tp_methods        */
	Obarr_members,   /* tp_members        */
	0,    /* tp_getset         */
	0,				/* tp_base           */
	0,				/* tp_dict           */
	0,				/* tp_descr_get      */
	0,				/* tp_descr_set      */
	0,				/* tp_dictoffset     */
	(initproc)Obarr_init,		/* tp_init           */
};


