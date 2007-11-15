#include <Python.h>
#include <structmember.h>

#if PY_VERSION_HEX < 0x02050000 && !defined(PY_SSIZE_T_MIN)
typedef int Py_ssize_t;
#define PY_SSIZE_T_MAX INT_MAX
#define PY_SSIZE_T_MIN INT_MIN
#endif

typedef struct rb_red_blk_tree rb_red_blk_tree;
typedef struct ObarrObject ObarrObject;

typedef struct CgridObject {
	PyObject_HEAD
	rb_red_blk_tree*	pTree;
	long				nSize;
	long				nCells;
	double				fCellSize;
	ObarrObject*		pUnrolled;
	int					bUnrollDirty;
} CgridObject;

staticforward PyTypeObject CgridObjectType;

#define Cgrid_Check(op) PyObject_TypeCheck(op, &CgridObjectType)
#define COPY_KEY(a, b) (b)->x = (a)->x; (b)->y = (a)->y; (b)->z = (a)->z;

typedef struct CgridKey {
	long x;
	long y;
	long z;
} CgridKey;
typedef struct CgridInfo {
	CgridObject* pSelf;
	CgridKey k;
	ObarrObject* pContents;
	
} CgridInfo;


/* internal functions */
void cgrid_unroll(CgridObject* self);
int cgrid_compare(const void* a, const void* b); 
CgridKey* cgrid_newkey();
CgridInfo* cgrid_newinfo();
void cgrid_destroykey(void* a);
void cgrid_destroyinfo(void* a);
void cgrid_printkey(const void* a);
void cgrid_printinfo(void* a);

/* exported API functions */
int Cgrid_init(CgridObject *self, PyObject *args, PyObject *kwds);
void Cgrid_dealloc(PyObject* self_in);
int Cgrid_contains(PyObject* self_in, PyObject* other_in);
PyObject * Cgrid_item(PyObject *self_in, Py_ssize_t index);
Py_ssize_t Cgrid_len(PyObject *self_in);
PyObject* Cgrid_repr(PyObject *self_in);
PyObject* Cgrid_insert(PyObject *self_in, PyObject *args);
PyObject* Cgrid_delete(PyObject *self_in, PyObject *args);
PyObject* Cgrid_remove(PyObject *self_in, PyObject *args);
/*PyObject* Cgrid_remove(PyObject *self_in, PyObject *args);*/


static PySequenceMethods Cgrid_as_seq[] = {
	Cgrid_len,			/* sq_length */
	0,					/* sq_concat */
	0,					/* sq_repeat */
	Cgrid_item,			/* sq_item */
	0,					/* sq_slice */
	0,					/* sq_ass_item */
	0,					/* sq_ass_slice */
	Cgrid_contains,		/* sq_contains */
};

static PyMethodDef Cgrid_methods[] = {
	{"insert", (PyCFunction)Cgrid_insert, METH_VARARGS, "allocate the array to a new size"},
	{"delete", (PyCFunction)Cgrid_delete, METH_VARARGS, "remove a grid cell"},
	{"remove", (PyCFunction)Cgrid_remove, METH_VARARGS, "remove an object from its grid cell"},
	{NULL}
};

static struct PyMemberDef Cgrid_members[] = {
	/*{"x", T_OBJECT_EX, offsetof(CgridObject, x), 0, "x"},
	{"y", T_OBJECT_EX, offsetof(CgridObject, y), 0, "y"},
	{"z", T_OBJECT_EX, offsetof(CgridObject, z), 0, "z"},*/
	{NULL}  /* Sentinel */
};


static PyTypeObject CgridObjectType = {
	PyObject_HEAD_INIT(NULL)
	0,				/* ob_size        */
	"pyobarr.cgrid",		/* tp_name        */
	sizeof(CgridObject),		/* tp_basicsize   */
	0,				/* tp_itemsize    */
	Cgrid_dealloc,	/* tp_dealloc     */
	0,				/* tp_print       */
	0,				/* tp_getattr     */
	0,				/* tp_setattr     */
	0,				/* tp_compare     */
	Cgrid_repr,	    /* tp_repr        */
	0,	/* tp_as_number   */
	Cgrid_as_seq,    /* tp_as_sequence */
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
	Cgrid_methods,   /* tp_methods        */
	Cgrid_members,   /* tp_members        */
	0,    /* tp_getset         */
	0,				/* tp_base           */
	0,				/* tp_dict           */
	0,				/* tp_descr_get      */
	0,				/* tp_descr_set      */
	0,				/* tp_dictoffset     */
	(initproc)Cgrid_init,		/* tp_init           */
};