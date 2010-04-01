#include "pos.h"
#include <math.h>

#ifdef _MSC_VER
#define isnan(x) ((x) != (x))
#define isinf(x) ((x) != (x))
#endif 

#define POSLEN 3

#define MATH_PI (atan(1.0)*4.0)
#define RAD2DEG (180.0 / MATH_PI)
#define DEG2RAD (MATH_PI / 180.0)



int Pos_init(PosObject *self, PyObject *args, PyObject *kwds)
{
	double inx, iny, inz;
    if (!PyArg_ParseTuple(args, "ddd", &inx, &iny, &inz))
        return -1;

	self->elements[0] = inx;
	self->elements[1] = iny;
	self->elements[2] = inz;
	
	return 0;
}

PyObject* pos_get_element(PyObject* self_in, long index)
{
	PosObject* self = (PosObject*)self_in;
	return PyFloat_FromDouble(self->elements[index]);
}

PyObject* Pos_getx(PyObject* self_in, void* closure)
{
	return pos_get_element(self_in, 0);
}

PyObject* Pos_gety(PyObject* self_in, void* closure)
{
	return pos_get_element(self_in, 1);
}

PyObject* Pos_getz(PyObject* self_in, void* closure)
{
	return pos_get_element(self_in, 2);
}

int Pos_set_notallowed(PyObject* self_in, PyObject* value, void* closure)
{
    PyErr_SetString(PyExc_TypeError, "Poss cannot be set directly");
	return -1;
}



PyObject* Pos_repr(PyObject *self_in)
{
	PosObject *self;
	PyObject *tuple, *fmtstring, *reprstring;
	
	if (!Pos_Check(self_in))
		return PyString_FromString("<unknown object type>");
	self = (PosObject*)self_in;
	
	tuple = Py_BuildValue("(ddd)", self->elements[0], self->elements[1], self->elements[2]);
	fmtstring = PyString_FromString("pos(%f, %f, %f)");
	reprstring = PyString_Format(fmtstring, tuple);
	Py_DECREF(tuple);
	Py_DECREF(fmtstring);
	return reprstring;

}


int Pos_true(PyObject *self_in)
{
	PosObject *self = (PosObject*)self_in;
	int b = 1;
	long i;
	double x;
	for (i = 0; i < POSLEN; i++)
	{
		x = self->elements[i];
		b = b && (x == 0.0 || isnan(x) || isinf(x));
	}
		
    return !b;
}

PyObject* Pos_add(PyObject *self_in, PyObject *other_in)
{
	PosObject *self, *other, *rv;
	long i;
	if (!Pos_Check(self_in) || !Pos_Check(other_in))	
	{
		PyErr_SetString(PyExc_TypeError, "both arguments must be of type 'pos'");
		return NULL;
	}
	self = (PosObject*)self_in;
	other = (PosObject*)other_in;
	rv = PyObject_New(PosObject, &PosObjectType);
	for (i = 0; i < POSLEN; i++)
		rv->elements[i] = self->elements[i] + other->elements[i];

	return (PyObject*)rv;
}

PyObject* Pos_sub(PyObject *self_in, PyObject *other_in)
{
	PosObject *self, *other, *rv;
	long i;
	if (!Pos_Check(self_in) || !Pos_Check(other_in))	
	{
		PyErr_SetString(PyExc_TypeError, "both arguments must be of type 'pos'");
		return NULL;
	}
	self = (PosObject*)self_in;
	other = (PosObject*)other_in;
	rv = PyObject_New(PosObject, &PosObjectType);
	for (i = 0; i < POSLEN; i++)
		rv->elements[i] = self->elements[i] - other->elements[i];

	return (PyObject*)rv;
}

PyObject* Pos_ip_add(PyObject *self_in, PyObject *other_in)
{
	PosObject *self, *other;
	long i;
	if (!Pos_Check(self_in) || !Pos_Check(other_in))	
	{
		PyErr_SetString(PyExc_TypeError, "both arguments must be of type 'pos'");
		return NULL;
	}
	self = (PosObject*)self_in;
	other = (PosObject*)other_in;
	for (i = 0; i < POSLEN; i++)
		self->elements[i] += other->elements[i];

	Py_INCREF(self);
	return (PyObject*)self;
}

PyObject* Pos_ip_sub(PyObject *self_in, PyObject *other_in)
{
	PosObject *self, *other;
	long i;

	if (!Pos_Check(self_in) || !Pos_Check(other_in))	
	{
		PyErr_SetString(PyExc_TypeError, "both arguments must be of type 'pos'");
		return NULL;
	}
	self = (PosObject*)self_in;
	other = (PosObject*)other_in;
	for (i = 0; i < POSLEN; i++)
		self->elements[i] -= other->elements[i];

	Py_INCREF(self);
	return (PyObject*)self;
}


P/*yObject* Pos_ip_zero(PyObject *self_in, PyObject *unused)
{
	PosObject *self;
	long i;
	if (!Pos_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "not a pos");
		return NULL;
	}
	self = (PosObject*)self_in;
	for (i = 0; i < POSLEN; i++)
		self->elements[i] = 0.0;

	Py_INCREF(self);
	return (PyObject*)self;
}
*/

PyObject* Pos_average(PyObject *self_in, PyObject *args)
{
	PosObject *self, *other, *rv;
	long i;
	if (!Pos_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "not a pos");
		return NULL;
	}
	self = (PosObject*)self_in;
  if (!PyArg_ParseTuple(args, "O!", &PosObjectType, &other))
	{
		PyErr_SetString(PyExc_TypeError, "argument is not a pos");
		return NULL;
	}
	rv = PyObject_New(PosObject, &PosObjectType);
	for (i = 0; i < POSLEN; i++)
		rv->elements[i] = (self->elements[i] + other->elements[i]) / 2.0;

	return (PyObject*)rv;
}

PyObject* Pos_dir(PyObject *self_in, PyObject *unused)
{
	PosObject *self;
	double d;
	long i;
	if (!Pos_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "not a pos");
		return NULL;
	}
	self = (PosObject*)self_in;
	d = 0.0;
	for (i = 0; i < POSLEN; i++)
		d += self->elements[i] * self->elements[i];

	return PyFloat_FromDouble(d);
}

PyObject* Pos_copy(PyObject *self_in, PyObject *unused)
{
	PosObject *self, *rv;
	long i;
	if (!Pos_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "not a pos");
		return NULL;
	}
	self = (PosObject*)self_in;
	rv = PyObject_New(PosObject, &PosObjectType);
	for (i = 0; i < POSLEN; i++)
		rv->elements[i] = self->elements[i];

	return (PyObject*)rv;
}

PyObject* Pos_dist(PyObject *self_in, PyObject *args)
{
	PosObject *self, *other;
	double d, dd;
	long i;
	if (!Pos_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "not a pos");
		return NULL;
	}
	self = (PosObject*)self_in;
  if (!PyArg_ParseTuple(args, "O!", &PosObjectType, &other))
	{
		PyErr_SetString(PyExc_TypeError, "argument is not a pos");
		return NULL;
	}
	d = 0.0;
	dd = 0.0;
	for (i = 0; i < POSLEN; i++)
	{
		dd = self->elements[i] - other->elements[i];
		d += dd * dd;
	}

	return PyFloat_FromDouble(sqrt(d));
}

PyObject* Pos_slerp(PyObject *self_in, PyObject *args)
{
	PosObject *self, *other, *rv;
	double amt, oamt;
	long i;
	if (!Pos_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "not a pos");
		return NULL;
	}
	self = (PosObject*)self_in;
	amt = 0.0;
  if (!PyArg_ParseTuple(args, "O!d", &PosObjectType, &other, &amt))
	{
		PyErr_SetString(PyExc_TypeError, "arguments must be a pos and a float");
		return NULL;
	}
	oamt = 1.0 - amt;
	rv = PyObject_New(PosObject, &PosObjectType);
	
	for (i = 0; i < POSLEN; i++)
	{
		rv->elements[i] = (self->elements[i] * oamt) + (other->elements[i] * amt);
	}

	return (PyObject*)rv;
}

PyObject* Pos_sserp(PyObject *self_in, PyObject *args)
{
	PosObject *self, *other, *rv, *norm;
	double amt, oamt, smag, omag;
	long i;
	if (!Pos_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "not a pos");
		return NULL;
	}
	self = (PosObject*)self_in;
	amt = 0.0;
  if (!PyArg_ParseTuple(args, "O!d", &PosObjectType, &other, &amt))
	{
		PyErr_SetString(PyExc_TypeError, "arguments must be a pos and a float");
		return NULL;
	}
	oamt = 1.0 - amt;
	rv = PyObject_New(PosObject, &PosObjectType);
	smag = 0.0;
	omag = 0.0;
	for (i = 0; i < POSLEN; i++)
	{
		smag += self->elements[i] * self->elements[i];
		omag += other->elements[i] * other->elements[i];
		rv->elements[i] = (self->elements[i] * oamt) + (other->elements[i] * amt);
	}
	smag = sqrt(smag);
	omag = sqrt(omag);

	norm = (PosObject*)Pos_ip_normalize((PyObject*)rv, NULL);
	Py_XDECREF(norm);
	
	for (i = 0; i < POSLEN; i++)
		rv->elements[i] *= (smag + omag) / 2.0;
	return (PyObject*)rv;
}


Py_ssize_t Pos_len(PyObject *self_in)
{
	if (!Pos_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "not a pos");
		return -1;
	}
	return POSLEN;
}


PyObject* Pos_item(PyObject *self_in, Py_ssize_t index)
{
	if (!Pos_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "not a pos");
		return NULL;
	}
	if (index < 0 || index >= POSLEN)
	{
		PyErr_SetString(PyExc_IndexError, "index not in range");
		return NULL;
	}
		
	return pos_get_element(self_in, index);

}




PyNumberMethods Pos_as_number[] = {
    Pos_add,           /* nb_add */
    Pos_sub,           /* nb_subtract */
    0,           /* nb_multiply */
    0,           /* nb_divide */
    0,                  /* nb_remainder */
    0,                  /* nb_divmod */
    0,                  /* nb_power */
    0,        /* nb_negative */
    0,                  /* nb_positive */
    0,                  /* nb_absolute */
    Pos_true,          /* nb_nonzero */
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
    Pos_ip_add,        /* nb_inplace_add */
    Pos_ip_sub,        /* nb_inplace_subtract */
    0,        /* nb_inplace_multiply */
    0,        /* nb_inplace_divide */
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

PySequenceMethods Pos_as_seq[] = {
	Pos_len,			/* sq_length */
	0,					/* sq_concat */
	0,					/* sq_repeat */
	Pos_item,			/* sq_item */
	0,					/* sq_slice */
	0,					/* sq_ass_item */
	0,					/* sq_ass_slice */
	0,					/* sq_contains */
};

PyGetSetDef Pos_getset[] = {
	{"x", Pos_getx, Pos_set_notallowed, "x", NULL},
	{"y", Pos_gety, Pos_set_notallowed, "y", NULL},
	{"z", Pos_getz, Pos_set_notallowed, "z", NULL},
	{NULL}
};

PyMethodDef Pos_methods[] = {
	{"__add__", (PyCFunction)Pos_add, METH_O|METH_COEXIST, "add two poss"},
	{"__sub__", (PyCFunction)Pos_sub, METH_O|METH_COEXIST, "subtract two poss"},
/*	{"zero", (PyCFunction)Pos_ip_zero, METH_NOARGS, "sets all pos components to 0"},*/
	{"avg", (PyCFunction)Pos_average, METH_VARARGS, "find halfway between this and another pos"},
	{"dist", (PyCFunction)Pos_dist, METH_VARARGS, "compute the distance between this and another pos"},
	{"copy", (PyCFunction)Pos_copy, METH_NOARGS, "makes a copy"},
	{"slerp", (PyCFunction)Pos_slerp, METH_VARARGS, "spherical linear interpolation"},
	{"sserp", (PyCFunction)Pos_sserp, METH_VARARGS, "spherical spherical interpolation"},
	{NULL}
};

struct PyMemberDef Pos_members[] = {
	/*{"x", T_OBJECT_EX, offsetof(PosObject, x), 0, "x"},
	{"y", T_OBJECT_EX, offsetof(PosObject, y), 0, "y"},
	{"z", T_OBJECT_EX, offsetof(PosObject, z), 0, "z"},*/
	{NULL}  /* Sentinel */
};


PyTypeObject PosObjectType = {
	PyObject_HEAD_INIT(NULL)
	0,				/* ob_size        */
	"py3dutil.pos",		/* tp_name        */
	sizeof(PosObject),		/* tp_basicsize   */
	0,				/* tp_itemsize    */
	0,				/* tp_dealloc     */
	0,				/* tp_print       */
	0,				/* tp_getattr     */
	0,				/* tp_setattr     */
	0,				/* tp_compare     */
	Pos_repr,	    /* tp_repr        */
	Pos_as_number,	/* tp_as_number   */
	Pos_as_seq,    /* tp_as_sequence */
	0,				/* tp_as_mapping  */
	0,				/* tp_hash        */
	0,				/* tp_call        */
	0,				/* tp_str         */
	0,				/* tp_getattro    */
	0,				/* tp_setattro    */
	0,				/* tp_as_buffer   */
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_CHECKTYPES,		/* tp_flags       */
	"Pos objects are simple.",	/* tp_doc         */
	0,				/* tp_traverse       */
	0,				/* tp_clear          */
	0,				/* tp_richcompare    */
	0,				/* tp_weaklistoffset */
	0,				/* tp_iter           */
	0,				/* tp_iternext       */
	Pos_methods,   /* tp_methods        */
	Pos_members,   /* tp_members        */
	Pos_getset,    /* tp_getset         */
	0,				/* tp_base           */
	0,				/* tp_dict           */
	0,				/* tp_descr_get      */
	0,				/* tp_descr_set      */
	0,				/* tp_dictoffset     */
	(initproc)Pos_init,		/* tp_init           */
};