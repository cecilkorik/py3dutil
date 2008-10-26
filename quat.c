#include "quat.h"
#include "vect.h"
#include <math.h>

#ifdef _MSC_VER
#define isnan(x) ((x) != (x))
#define isinf(x) ((x) != (x))
#endif 

#define MATH_PI (atan(1.0)*4.0)
#define RAD2DEG (180.0 / MATH_PI)
#define DEG2RAD (MATH_PI / 180.0)

int Quat_init(QuatObject *self, PyObject *args, PyObject *kwds)
{
	double inx, iny, inz, inw;
    if (!PyArg_ParseTuple(args, "dddd", &inx, &iny, &inz, &inw))
        return -1;

	self->elements[0] = inx;
	self->elements[1] = iny;
	self->elements[2] = inz;
	self->elements[3] = inw;
	
	return 0;
}

PyObject* quat_get_element(PyObject* self_in, long index)
{
	QuatObject* self = (QuatObject*)self_in;
	return PyFloat_FromDouble(self->elements[index]);
}

PyObject* Quat_getx(PyObject* self_in, void* closure)
{
	return quat_get_element(self_in, 0);
}

PyObject* Quat_gety(PyObject* self_in, void* closure)
{
	return quat_get_element(self_in, 1);
}

PyObject* Quat_getz(PyObject* self_in, void* closure)
{
	return quat_get_element(self_in, 2);
}

PyObject* Quat_getw(PyObject* self_in, void* closure)
{
	return quat_get_element(self_in, 3);
}

int
Quat_set_notallowed(PyObject* self_in, PyObject* value, void* closure)
{
    PyErr_SetString(PyExc_TypeError, "Quaternions cannot be set directly");
	return -1;
}

PyObject* Quat_repr(PyObject *self_in)
{
	QuatObject *self;
	PyObject *tuple, *fmtstring, *reprstring;
	
	if (!Quat_Check(self_in))
		return PyString_FromString("<unknown object type>");
	self = (QuatObject*)self_in;
	
	tuple = Py_BuildValue("(dddd)", self->elements[0], self->elements[1], self->elements[2], self->elements[3]);
	fmtstring = PyString_FromString("quat(%f, %f, %f, %f)");
	reprstring = PyString_Format(fmtstring, tuple);
	Py_DECREF(tuple);
	Py_DECREF(fmtstring);
	return reprstring;
}


int Quat_true(PyObject *self_in)
{
	QuatObject *self = (QuatObject*)self_in;
	int b = 1;
	int i;
	double x;
	for (i = 0; i < 4; i++)
	{
		x = self->elements[i];
		b = b && (x == 0.0 || isnan(x) || isinf(x));
	}
		
    return !b;
}

void quat_multiply_internal(QuatObject* q1, QuatObject* q2, QuatObject* qr)
{
#define x1 q1->elements[0]
#define x2 q2->elements[0]
#define y1 q1->elements[1]
#define y2 q2->elements[1]
#define z1 q1->elements[2]
#define z2 q2->elements[2]
#define	w1 q1->elements[3]
#define w2 q2->elements[3]

	qr->elements[0] = (w1 * x2) + (x1 * w2) + (y1 * z2) - (z1 * y2);
	qr->elements[1] = (w1 * y2) + (y1 * w2) + (z1 * x2) - (x1 * z2);
	qr->elements[2] = (w1 * z2) + (z1 * w2) + (x1 * y2) - (y1 * x2);
	qr->elements[3] = (w1 * w2) - (x1 * x2) - (y1 * y2) - (z1 * z2);

	quat_normalize_internal(qr);

#undef x1
#undef x2
#undef y1
#undef y2
#undef z1
#undef z2
#undef w1
#undef w2
}

double quat_mag2_internal(QuatObject* self)
{
	double rv = 0.0;
	int i;
	for (i = 0; i < 4; i++)
		rv += self->elements[i] * self->elements[i];
	return rv;
}

double quat_mag_internal(QuatObject *self)
{
	double rv = 0.0;
	int i;
	for (i = 0; i < 4; i++)
		rv += self->elements[i] * self->elements[i];
	return sqrt(rv);
}

void quat_get_conjugate_internal(QuatObject *self, QuatObject *rv)
{
	int i;
	for (i = 0; i < 3; i++)
		rv->elements[i] = -self->elements[i];
	rv->elements[3] = self->elements[3];
}

void quat_normalize_internal(QuatObject *self)
{
	int i;
	double mag = quat_mag2_internal(self);
	if (mag > (1.0 + -1e-7) && mag < (1.0 + 1e-7))
		return;
	mag = sqrt(mag);

	for (i = 0; i < 4; i++)
		self->elements[i] = self->elements[i] / mag;

}

void quat_multiply_vect_internal(QuatObject *self, VectObject *v, VectObject *rv)
{
	QuatObject *conj, *rq, *other;
	double mag = quat_mag_internal(other);
	int i;

	// used in calculation, must be freed!
	conj = PyObject_New(QuatObject, &QuatObjectType);
	rq = PyObject_New(QuatObject, &QuatObjectType);
	other = PyObject_New(QuatObject, &QuatObjectType);

	quat_get_conjugate_internal(self, conj);

	for (i = 0; i < 3; i++)
		rq->elements[i] = v->elements[i];
	quat_multiply_internal(self, rq, other);
	quat_multiply_internal(other, conj, rq);
	for (i = 0; i < 3; i++)
		rv->elements[i] = rq->elements[i] * mag;
	
	// free our calculation variables
	Py_DECREF(conj);
	Py_DECREF(other);
	Py_DECREF(rq);
}

PyObject* Quat_mul(PyObject *self_in, PyObject *other_in)
{
	QuatObject *self, *rq, *other;
	if (!Quat_Check(self_in))
	{
		PyErr_SetString(PyExc_TypeError, "incorrect object type received");
		return NULL;
	}
	
	if (!Quat_Check(other_in) && !Vect_Check(other_in))	
	
	self = (QuatObject*)self_in;

	if (Quat_Check(other_in))
	{
		other = (QuatObject*)other_in;
		rq = PyObject_New(QuatObject, &QuatObjectType);
		quat_multiply_internal(self, other, rq);
		return (PyObject*)rq;
	}
	else if (Vect_Check(other_in))
	{
		VectObject *v, *rv;
		v = (VectObject*)other_in;
		rv = PyObject_New(VectObject, &VectObjectType);

		quat_multiply_vect_internal(self, v, rv);
		return (PyObject*)rv;
	}
	else
	{
		PyErr_SetString(PyExc_TypeError, "quaternion can only be multiplied by a quaternion or vector");
		return NULL;
	}

}

PyObject* Quat_get_conjugate(PyObject *self_in, PyObject *unused)
{
	QuatObject *self, *rv;
	if (!Quat_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "incorrect object type");
		return NULL;
	}
	self = (QuatObject*)self_in;
	rv = PyObject_New(QuatObject, &QuatObjectType);
	quat_get_conjugate_internal(self, rv);

	return (PyObject*)rv;
}

PyObject* Quat_get_matrix(PyObject *self_in, PyObject *unused)
{
	QuatObject *self;
	int i;
	double mtx[16];
	double x, y, z, w;
	double xx, yy, zz;
	double xy, xz, yz;
	double xw, yw, zw;
	PyObject* list;

	if (!Quat_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "invalid object type");
		return NULL;
	}
	self = (QuatObject*)self_in;
	x = self->elements[0]; y = self->elements[1]; z = self->elements[2]; w = self->elements[3];
	xx = x * x; yy = y * y; zz = z * z;
	xy = x * y; xz = x * z; yz = y * z;
	xw = x * w; yw = y * w; zw = z * w;

	// matrix diagonal
	mtx[0] = 1.0 - (2.0 * (yy+zz));
	mtx[5] = 1.0 - (2.0 * (xx+zz));
	mtx[10] = 1.0 - (2.0 * (xx+yy));
	mtx[15] = 1.0;

	// rotation cells
	mtx[1] = 2.0 * (xy-zw);
	mtx[2] = 2.0 * (xz+yw);
	mtx[4] = 2.0 * (xy+zw);
	mtx[6] = 2.0 * (yz-xw);
	mtx[8] = 2.0 * (xz-yw);
	mtx[9] = 2.0 * (yz+xw);

	// right and bottom sides of matrix
	mtx[3] = 0.0;
	mtx[7] = 0.0;
	mtx[11] = 0.0;
	mtx[12] = 0.0;
	mtx[13] = 0.0;
	mtx[14] = 0.0;

	list = PyList_New(16);
	for (i = 0; i < 16; i++)
		PyList_SET_ITEM(list, i, PyFloat_FromDouble(mtx[i]));

	return (PyObject*)list;
}

PyObject* Quat_get_angle(PyObject *self_in, PyObject *unused)
{
	QuatObject *self, *other;
	int i;
	double x, y, z, w;
	double xx, yy, zz;
	double xy, xz, yz;
	double xw, yw, zw;
	double a[3];
	PyObject* list;

	if (!Quat_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "invalid object type");
		return NULL;
	}
	self = (QuatObject*)self_in;

	
	x = self->elements[0]; y = self->elements[1]; z = self->elements[2]; w = self->elements[3];
	xx = x * x; yy = y * y; zz = z * z;
	xy = x * y; xz = x * z; yz = y * z;
	xw = x * w; yw = y * w; zw = z * w;



	a[0] = atan((2.0 * (xw+yz)) / (1.0 - (2.0 * (xx+yy)))) * RAD2DEG;
	a[1] = asin(2.0 * (yw-xz)) * RAD2DEG;
	a[2] = atan((2.0 * (zw+xy)) / (1.0 - (2.0 * (yy+zz)))) * RAD2DEG;

	for (i = 0; i < 4; i++)
		self->elements[i] -= other->elements[i];

	list = PyList_New(3);
	for (i = 0; i < 3; i++)
		PyList_SET_ITEM(list, i, PyFloat_FromDouble(a[i]));

	return (PyObject*)list;
}

PyObject* Quat_slerp(PyObject *self_in, PyObject *args)
{
	QuatObject *self, *other, *rv;
	int i;
	double amt, ss, ts, cv, ang, as;

	if (!Quat_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "'vect' can only be multiplied by a scalar");
		return NULL;
	}
	if (!PyArg_ParseTuple(args, "O!d", &QuatObjectType, &other, &amt))
	{
		PyErr_SetString(PyExc_TypeError, "arguments must be a quat and a float");
		return NULL;
	}
	self = (QuatObject*)self_in;
	cv = 0.0;
	for (i = 0; i < 4; i++)
		cv += self->elements[i] * other->elements[i];
	if (cv < 0.0)
		cv = -cv;
	if (1.0 - cv > 1e-7)
	{
		ang = acos(cv);
		as = sin(ang);
		ss = sin((1.0 - amt) * ang) / as;
		ts = sin(amt * ang) / as;
	}
	else
	{
		ss = 1.0 - amt;
		ts = amt;
	}

	rv = PyObject_New(QuatObject, &QuatObjectType);
	for (i = 0; i < 4; i++)
		rv->elements[i] = (self->elements[i] * ss) + (other->elements[i] * ts);

	return (PyObject*)rv;
}

PyObject* Quat_slerp_turn(PyObject *self_in, PyObject *args)
{
	QuatObject *self, *other, *rv;
	VectObject *axis, *v1, *v2;
	int i;
	double amt, ss, ts, cv, ang, as;
	double diff;

	if (!Quat_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "'vect' can only be multiplied by a scalar");
		return NULL;
	}
	if (!PyArg_ParseTuple(args, "O!d", &QuatObjectType, &other, &amt))
	{
		PyErr_SetString(PyExc_TypeError, "arguments must be a quat and a float");
		return NULL;
	}

	// these need to be freed
	axis = PyObject_New(VectObject, &VectObjectType);
	v1 = PyObject_New(VectObject, &VectObjectType);
	v2 = PyObject_New(VectObject, &VectObjectType);

	axis->elements[0] = 0.0; axis->elements[1] = 1.0; axis->elements[2] = 0.0;
	quat_multiply_vect_internal(self, axis, v1);
	quat_multiply_vect_internal(other, axis, v2);

	diff = vect_dotprod_internal(v1, v2);
	Py_DECREF(v1);
	Py_DECREF(v2);
	Py_DECREF(axis);
	
	if (amt >= diff)
		return Quat_copy((PyObject*)other, NULL);

	cv = 0.0;
	for (i = 0; i < 4; i++)
		cv += self->elements[i] * other->elements[i];
	if (cv < 0.0)
		cv = -cv;
	if (1.0 - cv > 1e-7)
	{
		ang = acos(cv);
		as = sin(ang);
		ss = sin((1.0 - amt) * ang) / as;
		ts = sin(amt * ang) / as;
	}
	else
	{
		ss = 1.0 - amt;
		ts = amt;
	}

	rv = PyObject_New(QuatObject, &QuatObjectType);
	for (i = 0; i < 4; i++)
		rv->elements[i] = (self->elements[i] * ss) + (other->elements[i] * ts);

	return (PyObject*)rv;
}

PyObject* Quat_copy(PyObject *self_in, PyObject *unused)
{
	QuatObject *self, *rv;
	int i;
	if (!Quat_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "not a vector");
		return NULL;
	}
	self = (QuatObject*)self_in;
	rv = PyObject_New(QuatObject, &QuatObjectType);
	for (i = 0; i < 4; i++)
		rv->elements[i] = self->elements[i];

	return (PyObject*)rv;
}

PyObject* Quat_ip_normalize(PyObject *self_in, PyObject *unused)
{
	QuatObject *self;
	if (!Quat_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "not a vector");
		return NULL;
	}
	self = (QuatObject*)self_in;
	quat_normalize_internal(self);

	Py_INCREF(self);
	return (PyObject*)self;
}

PyObject* Quat_mag(PyObject *self_in, PyObject *unused)
{
	QuatObject *self;
	if (!Quat_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "not a quat");
		return NULL;
	}
	self = (QuatObject*)self_in;
	return PyFloat_FromDouble(quat_mag_internal(self));
}

PyObject* Quat_mag2(PyObject *self_in, PyObject *unused)
{
	QuatObject *self;
	if (!Quat_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "not a quat");
		return NULL;
	}
	self = (QuatObject*)self_in;
	return PyFloat_FromDouble(quat_mag_internal(self));
}

int Quat_len(PyObject *self_in)
{
	if (!Quat_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "not a quat");
		return -1;
	}
	return 4;
}


PyObject* Quat_item(PyObject *self_in, int index)
{
	if (!Quat_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "not a quat");
		return NULL;
	}
	if (index < 0 || index >= 4)
	{
		PyErr_SetString(PyExc_IndexError, "index not in range");
		return NULL;
	}
		
	return quat_get_element(self_in, index);

}

PyObject* Quat_richcompare(PyObject* a, PyObject* b, int op)
{
	VectObject *axis, *v1, *v2;
	QuatObject *self, *other;
	double diff;
	if (op == Py_EQ)
	{
		if (!Quat_Check(a) || !Quat_Check(b))	
		{
			PyErr_SetString(PyExc_TypeError, "can only compare two quats");
			return NULL;
		}
		self = (QuatObject*)a;
		other = (QuatObject*)b;

		// these need to be freed
		axis = PyObject_New(VectObject, &VectObjectType);
		v1 = PyObject_New(VectObject, &VectObjectType);
		v2 = PyObject_New(VectObject, &VectObjectType);

		axis->elements[0] = 0.0; axis->elements[1] = 1.0; axis->elements[2] = 0.0;
		quat_multiply_vect_internal(self, axis, v1);
		quat_multiply_vect_internal(other, axis, v2);

		diff = vect_dotprod_internal(v1, v2);
		Py_DECREF(v1);
		Py_DECREF(v2);
		Py_DECREF(axis);

		if (diff < 1e-9)
			return Py_True;
		else
			return Py_False;
		
	}
	return Py_NotImplemented;
}





PyNumberMethods Quat_as_number[] = {
    0,                  /* nb_add */
    0,                  /* nb_subtract */
    Quat_mul,           /* nb_multiply */
    0,                  /* nb_divide */
    0,                  /* nb_remainder */
    0,                  /* nb_divmod */
    0,                  /* nb_power */
    0,			        /* nb_negative */
    0,                  /* nb_positive */
    0,                  /* nb_absolute */
    Quat_true,          /* nb_nonzero */
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
    0,                  /* nb_inplace_add */
    0,                  /* nb_inplace_subtract */
    0,                  /* nb_inplace_multiply */
    0,                  /* nb_inplace_divide */
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

PySequenceMethods Quat_as_seq[] = {
	Quat_len,			/* sq_length */
	0,					/* sq_concat */
	0,					/* sq_repeat */
	Quat_item,			/* sq_item */
	0,					/* sq_slice */
	0,					/* sq_ass_item */
	0,					/* sq_ass_slice */
	0,					/* sq_contains */
};

PyGetSetDef Quat_getset[] = {
	{"x", Quat_getx, Quat_set_notallowed, "x", NULL},
	{"y", Quat_gety, Quat_set_notallowed, "y", NULL},
	{"z", Quat_getz, Quat_set_notallowed, "z", NULL},
	{"w", Quat_getw, Quat_set_notallowed, "w", NULL},
	{NULL}
};

PyMethodDef Quat_methods[] = {
	{"__mul__", (PyCFunction)Quat_mul, METH_O|METH_COEXIST, "multiply a quat by a quat or vector"},
	{"__neg__", (PyCFunction)Quat_get_conjugate, METH_O|METH_COEXIST, "get the conjugate of the quat"},
	{"normalize", (PyCFunction)Quat_ip_normalize, METH_NOARGS, "normalize a quaternion in place"},
	{"get_matrix", (PyCFunction)Quat_get_matrix, METH_NOARGS, "get quat in matrix form (for plugging into OpenGL)"},
	{"get_angle", (PyCFunction)Quat_get_angle, METH_NOARGS, "get the angles in Euler angles"},
	{"mag", (PyCFunction)Quat_mag, METH_NOARGS, "compute the vector magnitude"},
	{"mag2", (PyCFunction)Quat_mag2, METH_NOARGS, "compute the squared vector magnitude"},
	{"get_conjugate", (PyCFunction)Quat_get_conjugate, METH_NOARGS, "get the conjugate of the quat"},
	{"copy", (PyCFunction)Quat_copy, METH_NOARGS, "makes a copy"},
	{"slerp", (PyCFunction)Quat_slerp, METH_VARARGS, "spherical linear interpolation"},
	{"slerp_turn", (PyCFunction)Quat_slerp_turn, METH_VARARGS, "limited slerp"},
	{NULL}
};

struct PyMemberDef Quat_members[] = {
	{NULL}  /* Sentinel */
};


PyTypeObject QuatObjectType = {
	PyObject_HEAD_INIT(NULL)
	0,				/* ob_size        */
	"py3dutil.quat",		/* tp_name        */
	sizeof(QuatObject),		/* tp_basicsize   */
	0,				/* tp_itemsize    */
	0,				/* tp_dealloc     */
	0,				/* tp_print       */
	0,				/* tp_getattr     */
	0,				/* tp_setattr     */
	0,				/* tp_compare     */
	Quat_repr,	    /* tp_repr        */
	Quat_as_number,	/* tp_as_number   */
	Quat_as_seq,    /* tp_as_sequence */
	0,				/* tp_as_mapping  */
	0,				/* tp_hash        */
	0,				/* tp_call        */
	0,				/* tp_str         */
	0,				/* tp_getattro    */
	0,				/* tp_setattro    */
	0,				/* tp_as_buffer   */
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_CHECKTYPES|Py_TPFLAGS_HAVE_RICHCOMPARE,		/* tp_flags       */
	"Quaternion objects are simple.",	/* tp_doc         */
	0,				/* tp_traverse       */
	0,				/* tp_clear          */
	Quat_richcompare,				/* tp_richcompare    */
	0,				/* tp_weaklistoffset */
	0,				/* tp_iter           */
	0,				/* tp_iternext       */
	Quat_methods,   /* tp_methods        */
	Quat_members,   /* tp_members        */
	Quat_getset,    /* tp_getset         */
	0,				/* tp_base           */
	0,				/* tp_dict           */
	0,				/* tp_descr_get      */
	0,				/* tp_descr_set      */
	0,				/* tp_dictoffset     */
	(initproc)Quat_init,		/* tp_init           */
};
