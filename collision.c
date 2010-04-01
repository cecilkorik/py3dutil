#include "collision.h"

double sizes[] = {100.0, 10000.0, 1000000.0}

PyObject* py3dutil_collide(PyObject* self_in, PyObject* args)
{
	ColliderObject* grid;
    if (!PyArg_ParseTuple(args, "O!", &Collider))
    	return NULL;

	
	Collider_unroll(grid);
}






PySequenceMethods Collider_as_seq[] = {
	Collider_len,			/* sq_length */
	0,					/* sq_concat */
	0,					/* sq_repeat */
	Collider_item,			/* sq_item */
	0,					/* sq_slice */
	0,					/* sq_ass_item */
	0,					/* sq_ass_slice */
	Collider_contains,		/* sq_contains */
};

PyMethodDef Collider_methods[] = {
	{"insert", (PyCFunction)Collider_insert, METH_VARARGS, "allocate the array to a new size"},
	{"delete", (PyCFunction)Collider_delete, METH_VARARGS, "remove a grid cell"},
	{"remove", (PyCFunction)Collider_remove, METH_VARARGS, "remove an object from its grid cell"},
	{NULL}
};

struct PyMemberDef Collider_members[] = {
	/*{"x", T_OBJECT_EX, offsetof(ColliderObject, x), 0, "x"},
	{"y", T_OBJECT_EX, offsetof(ColliderObject, y), 0, "y"},
	{"z", T_OBJECT_EX, offsetof(ColliderObject, z), 0, "z"},*/
	{NULL}  /* Sentinel */
};


PyTypeObject ColliderObjectType = {
	PyObject_HEAD_INIT(NULL)
	0,				/* ob_size        */
	"py3dutil.Collider",		/* tp_name        */
	sizeof(ColliderObject),		/* tp_basicsize   */
	0,				/* tp_itemsize    */
	Collider_dealloc,	/* tp_dealloc     */
	0,				/* tp_print       */
	0,				/* tp_getattr     */
	0,				/* tp_setattr     */
	0,				/* tp_compare     */
	Collider_repr,	    /* tp_repr        */
	0,	/* tp_as_number   */
	Collider_as_seq,    /* tp_as_sequence */
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
	Collider_methods,   /* tp_methods        */
	Collider_members,   /* tp_members        */
	0,    /* tp_getset         */
	0,				/* tp_base           */
	0,				/* tp_dict           */
	0,				/* tp_descr_get      */
	0,				/* tp_descr_set      */
	0,				/* tp_dictoffset     */
	(initproc)Collider_init,		/* tp_init           */
};