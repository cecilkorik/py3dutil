#include <Python.h>
#include <structmember.h>

#if PY_VERSION_HEX < 0x02050000 && !defined(PY_SSIZE_T_MIN)
typedef int Py_ssize_t;
#define PY_SSIZE_T_MAX INT_MAX
#define PY_SSIZE_T_MIN INT_MIN
#endif

#define VEC3D

#ifdef VEC3D
#define VECLEN 3
#endif

#ifdef VEC2D
#define VECLEN 2
#endif


typedef struct VectObject {
	PyObject_HEAD
	double elements[VECLEN];
} VectObject;


#define Vect_Check(op) PyObject_TypeCheck(op, &VectObjectType)


// internal functions
PyObject* vect_get_element(PyObject* self_in, long index);

// Python API functions
int Vect_init(VectObject *self, PyObject *args, PyObject *kwds);
PyObject* Vect_getx(PyObject* self_in, void* closure);
PyObject* Vect_gety(PyObject* self_in, void* closure);
#ifdef VEC3D
PyObject* Vect_getz(PyObject* self_in, void* closure);
#endif
int Vect_set_notallowed(PyObject* self_in, PyObject* value, void* closure);
PyObject* Vect_repr(PyObject *self_in);
int Vect_true(PyObject *self_in);
PyObject* Vect_add(PyObject *self_in, PyObject *other_in);
PyObject* Vect_sub(PyObject *self_in, PyObject *other_in);
PyObject* Vect_mul(PyObject *self_in, PyObject *other_in);
PyObject* Vect_div(PyObject *self_in, PyObject *other_in);
PyObject* Vect_ip_add(PyObject *self_in, PyObject *other_in);
PyObject* Vect_ip_sub(PyObject *self_in, PyObject *other_in);
PyObject* Vect_ip_mul(PyObject *self_in, PyObject *other_in);
PyObject* Vect_ip_div(PyObject *self_in, PyObject *other_in);
PyObject* Vect_negate(PyObject *self_in);
PyObject* Vect_ip_negate(PyObject *self_in, PyObject *unused);
PyObject* Vect_ip_zero(PyObject *self_in, PyObject *unused);
PyObject* Vect_ip_normalize(PyObject *self_in, PyObject *unused);
PyObject* Vect_mag(PyObject *self_in, PyObject *unused);
PyObject* Vect_mag2(PyObject *self_in, PyObject *unused);
PyObject* Vect_dotprod(PyObject *self_in, PyObject *args);
PyObject* Vect_crossprod(PyObject *self_in, PyObject *unused);
PyObject* Vect_average(PyObject *self_in, PyObject *args);
PyObject* Vect_dir(PyObject *self_in, PyObject *unused);
PyObject* Vect_copy(PyObject *self_in, PyObject *unused);
PyObject* Vect_dist(PyObject *self_in, PyObject *args);
PyObject* Vect_slerp(PyObject *self_in, PyObject *args);
PyObject* Vect_sserp(PyObject *self_in, PyObject *args);
Py_ssize_t Vect_len(PyObject *self_in);
PyObject* Vect_item(PyObject *self_in, Py_ssize_t index);






static PyNumberMethods Vect_as_number[] = {
    Vect_add,           /* nb_add */
    Vect_sub,           /* nb_subtract */
    Vect_mul,           /* nb_multiply */
    Vect_div,           /* nb_divide */
    0,                  /* nb_remainder */
    0,                  /* nb_divmod */
    0,                  /* nb_power */
    Vect_negate,        /* nb_negative */
    0,                  /* nb_positive */
    0,                  /* nb_absolute */
    Vect_true,          /* nb_nonzero */
    0,                  /* nb_invert */
    0,                  /* nb_lshift */
    0,                  /* nb_rshift */
    0,                  /* nb_and */
    0,                  /* nb_xor */
    0,                  /* nb_or */
    0,                  /* nb_coerce */
    0,                  /* nb_int */
    0,                  /* nb_long */
    0,                  /* nb_float */
    0,                  /* nb_oct */
    0,                  /* nb_hex */
    Vect_ip_add,        /* nb_inplace_add */
    Vect_ip_sub,        /* nb_inplace_subtract */
    Vect_ip_mul,        /* nb_inplace_multiply */
    Vect_ip_div,        /* nb_inplace_divide */
    0,                  /* nb_inplace_remainder */
    0,                  /* nb_inplace_power */
    0,                  /* nb_inplace_lshift */
    0,                  /* nb_inplace_rshift */
    0,                  /* nb_inplace_and */
    0,                  /* nb_inplace_xor */
    0,                  /* nb_inplace_or */
    0,                  /* nb_floordiv */
    0,                  /* nb_truediv */
    0,                  /* nb_inplace_floordiv */
    0,                  /* nb_inplace_truediv */

};

static PySequenceMethods Vect_as_seq[] = {
	Vect_len,			/* sq_length */
	0,					/* sq_concat */
	0,					/* sq_repeat */
	Vect_item,			/* sq_item */
	0,					/* sq_slice */
	0,					/* sq_ass_item */
	0,					/* sq_ass_slice */
	0,					/* sq_contains */
};

static PyGetSetDef Vect_getset[] = {
	{"x", Vect_getx, Vect_set_notallowed, "x", NULL},
	{"y", Vect_gety, Vect_set_notallowed, "y", NULL},
#if defined (VEC3D)
	{"z", Vect_getz, Vect_set_notallowed, "z", NULL},
#endif
	{NULL}
};

static PyMethodDef Vect_methods[] = {
	{"__add__", (PyCFunction)Vect_add, METH_O|METH_COEXIST, "add two vectors"},
	{"__sub__", (PyCFunction)Vect_sub, METH_O|METH_COEXIST, "subtract two vectors"},
	{"__mul__", (PyCFunction)Vect_mul, METH_O|METH_COEXIST, "multiply a vector by a scalar"},
	{"__div__", (PyCFunction)Vect_div, METH_O|METH_COEXIST, "divide a vector by a scalar"},
	{"__neg__", (PyCFunction)Vect_negate, METH_O|METH_COEXIST, "negate (reverse) a vector"},
	{"zero", (PyCFunction)Vect_ip_zero, METH_NOARGS, "sets all vector components to 0"},
	{"negate", (PyCFunction)Vect_ip_negate, METH_NOARGS, "negate (reverse) a vector in place"},
	{"normalize", (PyCFunction)Vect_ip_normalize, METH_NOARGS, "normalize a vector in place"},
	{"avg", (PyCFunction)Vect_average, METH_VARARGS, "find halfway between this and another vector"},
	{"dot", (PyCFunction)Vect_dotprod, METH_VARARGS, "compute the dot product of this and another vector"},
	{"cross", (PyCFunction)Vect_crossprod, METH_VARARGS, "compute the cross product of this and another vector"},
	{"dist", (PyCFunction)Vect_dist, METH_VARARGS, "compute the distance between this and another vector"},
	{"mag", (PyCFunction)Vect_mag, METH_NOARGS, "compute the vector magnitude"},
	{"mag2", (PyCFunction)Vect_mag2, METH_NOARGS, "compute the squared vector magnitude"},
	{"dir", (PyCFunction)Vect_dir, METH_NOARGS, "compute the vector direction (in Euler angles)"},
	{"copy", (PyCFunction)Vect_copy, METH_NOARGS, "makes a copy"},
	{"slerp", (PyCFunction)Vect_slerp, METH_VARARGS, "spherical linear interpolation"},
	{"sserp", (PyCFunction)Vect_sserp, METH_VARARGS, "spherical spherical interpolation"},
	{NULL}
};

static struct PyMemberDef Vect_members[] = {
	/*{"x", T_OBJECT_EX, offsetof(VectObject, x), 0, "x"},
	{"y", T_OBJECT_EX, offsetof(VectObject, y), 0, "y"},
	{"z", T_OBJECT_EX, offsetof(VectObject, z), 0, "z"},*/
	{NULL}  /* Sentinel */
};


static PyTypeObject VectObjectType = {
	PyObject_HEAD_INIT(NULL)
	0,				/* ob_size        */
	"py3dutil.vect",		/* tp_name        */
	sizeof(VectObject),		/* tp_basicsize   */
	0,				/* tp_itemsize    */
	0,				/* tp_dealloc     */
	0,				/* tp_print       */
	0,				/* tp_getattr     */
	0,				/* tp_setattr     */
	0,				/* tp_compare     */
	Vect_repr,	    /* tp_repr        */
	Vect_as_number,	/* tp_as_number   */
	Vect_as_seq,    /* tp_as_sequence */
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
	Vect_methods,   /* tp_methods        */
	Vect_members,   /* tp_members        */
	Vect_getset,    /* tp_getset         */
	0,				/* tp_base           */
	0,				/* tp_dict           */
	0,				/* tp_descr_get      */
	0,				/* tp_descr_set      */
	0,				/* tp_dictoffset     */
	(initproc)Vect_init,		/* tp_init           */
};