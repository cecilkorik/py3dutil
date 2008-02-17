#include "vect.h"
#include <math.h>

#ifdef _MSC_VER
#define isnan(x) ((x) != (x))
#define isinf(x) ((x) != (x))
#endif 

#define VEC3D

#ifdef VEC3D
#define VECLEN 3
#endif

#ifdef VEC2D
#define VECLEN 2
#endif

#define MATH_PI (atan(1.0)*4.0)
#define RAD2DEG (180.0 / MATH_PI)
#define DEG2RAD (MATH_PI / 180.0)



int Vect_init(VectObject *self, PyObject *args, PyObject *kwds)
{
#if defined (VEC3D)
	double inx, iny, inz;
    if (!PyArg_ParseTuple(args, "ddd", &inx, &iny, &inz))
        return -1;

	self->elements[0] = inx;
	self->elements[1] = iny;
	self->elements[2] = inz;
#elif defined (VEC2D)
	double inx, iny;
    if (!PyArg_ParseTuple(args, "dd", &inx, &iny))
        return -1;

	self->elements[0] = inx;
	self->elements[1] = iny;
#endif
	
	return 0;
}

PyObject* vect_get_element(PyObject* self_in, long index)
{
	VectObject* self = (VectObject*)self_in;
	return PyFloat_FromDouble(self->elements[index]);
}

PyObject* Vect_getx(PyObject* self_in, void* closure)
{
	return vect_get_element(self_in, 0);
}

PyObject* Vect_gety(PyObject* self_in, void* closure)
{
	return vect_get_element(self_in, 1);
}

#if defined (VEC3D)
PyObject* Vect_getz(PyObject* self_in, void* closure)
{
	return vect_get_element(self_in, 2);
}
#endif

int Vect_set_notallowed(PyObject* self_in, PyObject* value, void* closure)
{
    PyErr_SetString(PyExc_TypeError, "Vectors cannot be set directly");
	return -1;
}



PyObject* Vect_repr(PyObject *self_in)
{
	VectObject *self;
	PyObject *tuple, *fmtstring, *reprstring;
	
	if (!Vect_Check(self_in))
		return PyString_FromString("<unknown object type>");
	self = (VectObject*)self_in;
	
#if defined (VEC3D)
	tuple = Py_BuildValue("(ddd)", self->elements[0], self->elements[1], self->elements[2]);
	fmtstring = PyString_FromString("vect(%f, %f, %f)");
	reprstring = PyString_Format(fmtstring, tuple);
	Py_DECREF(tuple);
	Py_DECREF(fmtstring);
	return reprstring;
#else
	return PyString_FromString("<unknown object type>");
#endif
}


int Vect_true(PyObject *self_in)
{
	VectObject *self = (VectObject*)self_in;
	int b = 1;
	long i;
	double x;
	for (i = 0; i < VECLEN; i++)
	{
		x = self->elements[i];
		b = b && (x == 0.0 || isnan(x) || isinf(x));
	}
		
    return !b;
}

PyObject* Vect_add(PyObject *self_in, PyObject *other_in)
{
	VectObject *self, *other, *rv;
	long i;
	if (!Vect_Check(self_in) || !Vect_Check(other_in))	
	{
		PyErr_SetString(PyExc_TypeError, "both arguments must be of type 'vect'");
		return NULL;
	}
	self = (VectObject*)self_in;
	other = (VectObject*)other_in;
	rv = PyObject_New(VectObject, &VectObjectType);
	for (i = 0; i < VECLEN; i++)
		rv->elements[i] = self->elements[i] + other->elements[i];

	return (PyObject*)rv;
}

PyObject* Vect_sub(PyObject *self_in, PyObject *other_in)
{
	VectObject *self, *other, *rv;
	long i;
	if (!Vect_Check(self_in) || !Vect_Check(other_in))	
	{
		PyErr_SetString(PyExc_TypeError, "both arguments must be of type 'vect'");
		return NULL;
	}
	self = (VectObject*)self_in;
	other = (VectObject*)other_in;
	rv = PyObject_New(VectObject, &VectObjectType);
	for (i = 0; i < VECLEN; i++)
		rv->elements[i] = self->elements[i] - other->elements[i];

	return (PyObject*)rv;
}

PyObject* Vect_mul(PyObject *self_in, PyObject *other_in)
{
	VectObject *self, *rv;
	long i;
	double scalar;
	if (!Vect_Check(self_in) || !PyFloat_Check(other_in))	
	{
		PyErr_SetString(PyExc_TypeError, "'vect' can only be multiplied by a scalar");
		return NULL;
	}
	
	self = (VectObject*)self_in;
	scalar = PyFloat_AsDouble(other_in);
	rv = PyObject_New(VectObject, &VectObjectType);
	for (i = 0; i < VECLEN; i++)
		rv->elements[i] = self->elements[i] * scalar;

	return (PyObject*)rv;
}

PyObject* Vect_div(PyObject *self_in, PyObject *other_in)
{
	VectObject *self, *rv;
	long i;
	double scalar;
	if (!Vect_Check(self_in) || !PyFloat_Check(other_in))	
	{
		PyErr_SetString(PyExc_TypeError, "'vect' can only be divided by a scalar");
		return NULL;
	}
	self = (VectObject*)self_in;
	scalar = PyFloat_AsDouble(other_in);
	rv = PyObject_New(VectObject, &VectObjectType);
	for (i = 0; i < VECLEN; i++)
		rv->elements[i] = self->elements[i] / scalar;

	return (PyObject*)rv;
}

PyObject* Vect_ip_add(PyObject *self_in, PyObject *other_in)
{
	VectObject *self, *other;
	long i;
	if (!Vect_Check(self_in) || !Vect_Check(other_in))	
	{
		PyErr_SetString(PyExc_TypeError, "both arguments must be of type 'vect'");
		return NULL;
	}
	self = (VectObject*)self_in;
	other = (VectObject*)other_in;
	for (i = 0; i < VECLEN; i++)
		self->elements[i] += other->elements[i];

	Py_INCREF(self);
	return (PyObject*)self;
}

PyObject* Vect_ip_sub(PyObject *self_in, PyObject *other_in)
{
	VectObject *self, *other;
	long i;

	if (!Vect_Check(self_in) || !Vect_Check(other_in))	
	{
		PyErr_SetString(PyExc_TypeError, "both arguments must be of type 'vect'");
		return NULL;
	}
	self = (VectObject*)self_in;
	other = (VectObject*)other_in;
	for (i = 0; i < VECLEN; i++)
		self->elements[i] -= other->elements[i];

	Py_INCREF(self);
	return (PyObject*)self;
}

PyObject* Vect_ip_mul(PyObject *self_in, PyObject *other_in)
{
	VectObject *self;
	long i;
	double scalar;
	if (!Vect_Check(self_in) || !PyFloat_Check(other_in))	
	{
		PyErr_SetString(PyExc_TypeError, "'vect' can only be multiplied by a scalar");
		return NULL;
	}
	self = (VectObject*)self_in;
	scalar = PyFloat_AsDouble(other_in);
	for (i = 0; i < VECLEN; i++)
		self->elements[i] *= scalar;

	Py_INCREF(self);
	return (PyObject*)self;
}

PyObject* Vect_ip_div(PyObject *self_in, PyObject *other_in)
{
	VectObject *self;
	long i;
	double scalar;
	if (!Vect_Check(self_in) || !PyFloat_Check(other_in))	
	{
		PyErr_SetString(PyExc_TypeError, "'vect' can only be divided by a scalar");
		return NULL;
	}
	self = (VectObject*)self_in;
	scalar = PyFloat_AsDouble(other_in);
	for (i = 0; i < VECLEN; i++)
		self->elements[i] /= scalar;

	Py_INCREF(self);
	return (PyObject*)self;
}

PyObject* Vect_negate(PyObject *self_in)
{
	VectObject *self, *rv;
	long i;
	if (!Vect_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "not a vector");
		return NULL;
	}
	self = (VectObject*)self_in;
	rv = PyObject_New(VectObject, &VectObjectType);
	for (i = 0; i < VECLEN; i++)
		rv->elements[i] = -self->elements[i];

	return (PyObject*)rv;
}

PyObject* Vect_ip_negate(PyObject *self_in, PyObject *unused)
{
	VectObject *self;
	long i;
	if (!Vect_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "not a vector");
		return NULL;
	}
	self = (VectObject*)self_in;
	for (i = 0; i < VECLEN; i++)
		self->elements[i] = -self->elements[i];

	Py_INCREF(self);
	return (PyObject*)self;
}

PyObject* Vect_ip_zero(PyObject *self_in, PyObject *unused)
{
	VectObject *self;
	long i;
	if (!Vect_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "not a vector");
		return NULL;
	}
	self = (VectObject*)self_in;
	for (i = 0; i < VECLEN; i++)
		self->elements[i] = 0.0;

	Py_INCREF(self);
	return (PyObject*)self;
}

PyObject* Vect_ip_normalize(PyObject *self_in, PyObject *unused)
{
	VectObject *self;
	double mag, mag2;
	long i;
	if (!Vect_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "not a vector");
		return NULL;
	}
	self = (VectObject*)self_in;
	mag2 = 0.0;
	for (i = 0; i < VECLEN; i++)
		mag2 += self->elements[i] * self->elements[i];
	if ((1.0 - mag2) < -0.001 || (1.0 - mag2) > 0.001)
	{
		mag = sqrt(mag2);
		for (i = 0; i < VECLEN; i++)
			self->elements[i] /= mag;
	}

	Py_INCREF(self);
	return (PyObject*)self;
}

PyObject* Vect_mag(PyObject *self_in, PyObject *unused)
{
	VectObject *self;
	double d;
	long i;
	if (!Vect_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "not a vector");
		return NULL;
	}
	self = (VectObject*)self_in;
	d = 0.0;
	for (i = 0; i < VECLEN; i++)
		d += self->elements[i] * self->elements[i];
	d = sqrt(d);

	return PyFloat_FromDouble(d);
}

PyObject* Vect_mag2(PyObject *self_in, PyObject *unused)
{
	VectObject *self;
	double d;
	long i;
	if (!Vect_Check(self_in))
	{
		PyErr_SetString(PyExc_TypeError, "not a vector");
		return NULL;
	}
	self = (VectObject*)self_in;
	d = 0.0;
	for (i = 0; i < VECLEN; i++)
		d += self->elements[i] * self->elements[i];

	return PyFloat_FromDouble(d);
}

PyObject* Vect_dotprod(PyObject *self_in, PyObject *args)
{
	VectObject *self, *other;
	double d;
	long i;
	if (!Vect_Check(self_in))
	{
		PyErr_SetString(PyExc_TypeError, "not a vector");
		return NULL;
	}
	self = (VectObject*)self_in;
	if (!PyArg_ParseTuple(args, "O!", &VectObjectType, &other))
	{
		PyErr_SetString(PyExc_TypeError, "argument is not a vector");
		return NULL;
	}
/* Python code is:
        value = sum([self[i] * other[i] for i in range(3)])
        if value >= 1.0:
            return 0.0
        return math.acos(value) * 180.0 / math.pi
*/	
	
	d = 0.0;
	for (i = 0; i < VECLEN; i++)
		d += self->elements[i] * other->elements[i];
	if (d >= 1.0)
		return PyFloat_FromDouble(0.0);

	return PyFloat_FromDouble(acos(d) * RAD2DEG);
}

#define A1 self->elements[0]
#define A2 self->elements[1]
#define A3 self->elements[2]
#define B1 other->elements[0]
#define B2 other->elements[1]
#define B3 other->elements[2]
PyObject* Vect_crossprod(PyObject *self_in, PyObject *args)
{
	VectObject *self, *other, *rv;
	if (!Vect_Check(self_in))
	{
		PyErr_SetString(PyExc_TypeError, "not a vector");
		return NULL;
	}
	self = (VectObject*)self_in;
	if (!PyArg_ParseTuple(args, "O!", &VectObjectType, &other))
	{
		PyErr_SetString(PyExc_TypeError, "argument is not a vector");
		return NULL;
	}
	rv = PyObject_New(VectObject, &VectObjectType);
	rv->elements[0] = (A2*B3) - (A3*B2);
	rv->elements[1] = (A3*B1) - (A1*B3);
	rv->elements[2] = (A1*B2) - (A2*B1);
	return (PyObject*)rv;
}

PyObject* Vect_average(PyObject *self_in, PyObject *args)
{
	VectObject *self, *other, *rv;
	long i;
	if (!Vect_Check(self_in))
	{
		PyErr_SetString(PyExc_TypeError, "not a vector");
		return NULL;
	}
	self = (VectObject*)self_in;
  if (!PyArg_ParseTuple(args, "O!", &VectObjectType, &other))
	{
		PyErr_SetString(PyExc_TypeError, "argument is not a vector");
		return NULL;
	}
	rv = PyObject_New(VectObject, &VectObjectType);
	for (i = 0; i < VECLEN; i++)
		rv->elements[i] = (self->elements[i] + other->elements[i]) / 2.0;

	return (PyObject*)rv;
}

PyObject* Vect_dir(PyObject *self_in, PyObject *unused)
{
	VectObject *self;
	double d;
	long i;
	if (!Vect_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "not a vector");
		return NULL;
	}
	self = (VectObject*)self_in;
	d = 0.0;
	for (i = 0; i < VECLEN; i++)
		d += self->elements[i] * self->elements[i];

	return PyFloat_FromDouble(d);
}

PyObject* Vect_copy(PyObject *self_in, PyObject *unused)
{
	VectObject *self, *rv;
	long i;
	if (!Vect_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "not a vector");
		return NULL;
	}
	self = (VectObject*)self_in;
	rv = PyObject_New(VectObject, &VectObjectType);
	for (i = 0; i < VECLEN; i++)
		rv->elements[i] = self->elements[i];

	return (PyObject*)rv;
}

PyObject* Vect_dist(PyObject *self_in, PyObject *args)
{
	VectObject *self, *other;
	double d, dd;
	long i;
	if (!Vect_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "not a vector");
		return NULL;
	}
	self = (VectObject*)self_in;
  if (!PyArg_ParseTuple(args, "O!", &VectObjectType, &other))
	{
		PyErr_SetString(PyExc_TypeError, "argument is not a vector");
		return NULL;
	}
	d = 0.0;
	dd = 0.0;
	for (i = 0; i < VECLEN; i++)
	{
		dd = self->elements[i] - other->elements[i];
		d += dd * dd;
	}

	return PyFloat_FromDouble(sqrt(d));
}

PyObject* Vect_slerp(PyObject *self_in, PyObject *args)
{
	VectObject *self, *other, *rv;
	double amt, oamt;
	long i;
	if (!Vect_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "not a vector");
		return NULL;
	}
	self = (VectObject*)self_in;
	amt = 0.0;
  if (!PyArg_ParseTuple(args, "O!d", &VectObjectType, &other, &amt))
	{
		PyErr_SetString(PyExc_TypeError, "arguments must be a vector and a float");
		return NULL;
	}
	oamt = 1.0 - amt;
	rv = PyObject_New(VectObject, &VectObjectType);
	
	for (i = 0; i < VECLEN; i++)
	{
		rv->elements[i] = (self->elements[i] * oamt) + (other->elements[i] * amt);
	}

	return (PyObject*)rv;
}

PyObject* Vect_sserp(PyObject *self_in, PyObject *args)
{
	VectObject *self, *other, *rv, *norm;
	double amt, oamt, smag, omag;
	long i;
	if (!Vect_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "not a vector");
		return NULL;
	}
	self = (VectObject*)self_in;
	amt = 0.0;
  if (!PyArg_ParseTuple(args, "O!d", &VectObjectType, &other, &amt))
	{
		PyErr_SetString(PyExc_TypeError, "arguments must be a vector and a float");
		return NULL;
	}
	oamt = 1.0 - amt;
	rv = PyObject_New(VectObject, &VectObjectType);
	smag = 0.0;
	omag = 0.0;
	for (i = 0; i < VECLEN; i++)
	{
		smag += self->elements[i] * self->elements[i];
		omag += other->elements[i] * other->elements[i];
		rv->elements[i] = (self->elements[i] * oamt) + (other->elements[i] * amt);
	}
	smag = sqrt(smag);
	omag = sqrt(omag);

	norm = (VectObject*)Vect_ip_normalize((PyObject*)rv, NULL);
	Py_XDECREF(norm);
	
	for (i = 0; i < VECLEN; i++)
		rv->elements[i] *= (smag + omag) / 2.0;
	return (PyObject*)rv;
}


Py_ssize_t Vect_len(PyObject *self_in)
{
	if (!Vect_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "not a vector");
		return -1;
	}
	return VECLEN;
}


PyObject* Vect_item(PyObject *self_in, Py_ssize_t index)
{
	if (!Vect_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "not a vector");
		return NULL;
	}
	if (index < 0 || index >= VECLEN)
	{
		PyErr_SetString(PyExc_IndexError, "index not in range");
		return NULL;
	}
		
	return vect_get_element(self_in, index);

}




PyNumberMethods Vect_as_number[] = {
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

PySequenceMethods Vect_as_seq[] = {
	Vect_len,			/* sq_length */
	0,					/* sq_concat */
	0,					/* sq_repeat */
	Vect_item,			/* sq_item */
	0,					/* sq_slice */
	0,					/* sq_ass_item */
	0,					/* sq_ass_slice */
	0,					/* sq_contains */
};

PyGetSetDef Vect_getset[] = {
	{"x", Vect_getx, Vect_set_notallowed, "x", NULL},
	{"y", Vect_gety, Vect_set_notallowed, "y", NULL},
#if defined (VEC3D)
	{"z", Vect_getz, Vect_set_notallowed, "z", NULL},
#endif
	{NULL}
};

PyMethodDef Vect_methods[] = {
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

struct PyMemberDef Vect_members[] = {
	/*{"x", T_OBJECT_EX, offsetof(VectObject, x), 0, "x"},
	{"y", T_OBJECT_EX, offsetof(VectObject, y), 0, "y"},
	{"z", T_OBJECT_EX, offsetof(VectObject, z), 0, "z"},*/
	{NULL}  /* Sentinel */
};


PyTypeObject VectObjectType = {
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