/*
	xray.jit.water
	Wesley Smith
	wesley.hoke@gmail.com

	last modified: 12-7-2006
*/

#include "jit.common.h"
#include "max.jit.mop.h"

#define H20_LAST_MATRIX	0
#define H20_LAST_VAL	1

typedef struct _max_xray_jit_water
{
	t_object		ob;
	void			*obex;
	float			val;
	long 				last;
	t_jit_matrix_info	lastinfo;

} t_max_xray_jit_water;

t_jit_err xray_jit_water_init(void);

void *max_xray_jit_water_new(t_symbol *s, long argc, t_atom *argv);
void max_xray_jit_water_free(t_max_xray_jit_water *x);
void *max_xray_jit_water_class;

void max_xray_jit_water_float(t_max_xray_jit_water *x, double f);
void max_xray_jit_water_val(t_max_xray_jit_water *x, void *attr, short argc, t_atom *argv);
void max_xray_jit_water_jit_matrix(t_max_xray_jit_water *x, t_symbol *s, short argc, t_atom *argv);
void max_xray_jit_water_mproc(t_max_xray_jit_water *x, void *mop);

void C74_EXPORT main(void)
{
	void *p,*q,*attr;
	long attrflags;

	xray_jit_water_init();
	setup((t_messlist **)&max_xray_jit_water_class, max_xray_jit_water_new, (method)max_xray_jit_water_free, (short)sizeof(t_max_xray_jit_water),
		0L, A_GIMME, 0);

	p = max_jit_classex_setup(calcoffset(t_max_xray_jit_water,obex));
	q = jit_class_findbyname(gensym("xray_jit_water"));

	addmess((method)max_xray_jit_water_jit_matrix, "jit_matrix", A_GIMME, 0);
    max_jit_classex_mop_wrap(p,q,MAX_JIT_MOP_FLAGS_OWN_JIT_MATRIX);
    max_jit_classex_mop_mproc(p,q,max_xray_jit_water_mproc); 	//custom mproc
    max_jit_classex_standard_wrap(p,q,0);

    attrflags = JIT_ATTR_GET_DEFER_LOW | JIT_ATTR_SET_USURP_LOW ;
	attr = jit_object_new(_jit_sym_jit_attr_offset,"val",_jit_sym_float32,attrflags,
		(method)0,(method)max_xray_jit_water_val,calcoffset(t_max_xray_jit_water,val),calcoffset(t_max_xray_jit_water,val));
	max_jit_classex_addattr(p,attr);

    addfloat((method)max_xray_jit_water_float);
    addmess((method)max_jit_mop_assist, "assist", A_CANT,0);
}

void max_xray_jit_water_float(t_max_xray_jit_water *x, double f)
{
	t_atom a;

	jit_atom_setfloat(&a,f);
	typedmess((t_object *)x,_jit_sym_val,1,&a);
}

void max_xray_jit_water_val(t_max_xray_jit_water *x, void *attr, short argc, t_atom *argv)
{
	void *o;

	if (o=max_jit_mop_getinput(x,2)) {
		jit_object_method(o,_jit_sym_getinfo,&x->lastinfo);
		jit_object_method(o,_jit_sym_setall,0L,(long)argc,argv);
		x->last = H20_LAST_VAL;
		x->val = jit_atom_getfloat(&argv[0]);
	}
}

void max_xray_jit_water_jit_matrix(t_max_xray_jit_water *x, t_symbol *s, short argc, t_atom *argv)
{
	if (max_jit_obex_inletnumber_get(x)) {
		x->last = H20_LAST_MATRIX;
	}
	max_jit_mop_jit_matrix(x,s,argc,argv);
}

void max_xray_jit_water_mproc(t_max_xray_jit_water *x, void *mop)
{
	t_jit_err err;
	t_jit_matrix_info tmpinfo;
	void *o;
	long changed=0;
	t_atom a;

	if (x->last==H20_LAST_VAL) {
		tmpinfo = x->lastinfo;
		o = max_jit_mop_getinput(x,2);
		jit_object_method(o,_jit_sym_getinfo,&x->lastinfo);
		if (tmpinfo.type!=x->lastinfo.type)
			changed = TRUE;
		else if (tmpinfo.planecount!=x->lastinfo.planecount)
			changed = TRUE;

		if (changed)
			jit_object_method(o,_jit_sym_setall,0L,1,jit_atom_setfloat(&a,x->val));
	}

	if (err=(t_jit_err) jit_object_method(
		max_jit_obex_jitob_get(x),
		_jit_sym_matrix_calc,
		jit_object_method(mop,_jit_sym_getinputlist),
		jit_object_method(mop,_jit_sym_getoutputlist)))
	{
		jit_error_code(x,err);
	} else {
		max_jit_mop_outputmatrix(x);
	}
}

void max_xray_jit_water_free(t_max_xray_jit_water *x)
{
	max_jit_mop_free(x);
	jit_object_free(max_jit_obex_jitob_get(x));
	max_jit_obex_free(x);
}

void *max_xray_jit_water_new(t_symbol *s, long argc, t_atom *argv)
{
	t_max_xray_jit_water *x;
	void *o;

	if (x=(t_max_xray_jit_water *)max_jit_obex_new(max_xray_jit_water_class,gensym("xray_jit_water"))) {
		if (o=jit_object_new(gensym("xray_jit_water"))) {
			max_jit_mop_setup_simple(x,o,argc,argv);
			max_jit_attr_args(x,argc,argv);
		} else {
			error("xray.jit.water: could not allocate object");
			freeobject((t_object *)x);
		}
	}
	return (x);
}
