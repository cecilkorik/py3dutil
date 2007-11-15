
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



staticforward PyTypeObject VectObjectType;

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

PyObject* vect_get_element(PyObject* self_in, int index)
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
	int i;
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
	int i;
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
	int i;
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
	int i;
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
	int i;
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
	int i;
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
	int i;

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
	int i;
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
	int i;
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
	int i;
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
	int i;
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
	int i;
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
	int i;
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
	int i;
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
	int i;
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
	int i;
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

PyObject* Vect_crossprod(PyObject *self_in, PyObject *unused)
{
	VectObject *self;
	double d;
	int i;
	if (!Vect_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "not a vector");
		return NULL;
	}
	self = (VectObject*)self_in;
	d = 0.0;
	for (i = 0; i < VECLEN; i++)
		d += self->elements[i] * self->elements[i];

	Py_INCREF(self);
	return (PyObject*)self;
}

PyObject* Vect_average(PyObject *self_in, PyObject *args)
{
	VectObject *self, *other, *rv;
	int i;
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
	int i;
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
	int i;
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
	int i;
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
	int i;
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
	int i;
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


int Vect_len(PyObject *self_in)
{
	if (!Vect_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "not a vector");
		return -1;
	}
	return VECLEN;
}


PyObject* Vect_item(PyObject *self_in, int index)
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



