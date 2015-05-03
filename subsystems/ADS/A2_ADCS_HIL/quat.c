
#include "quat.h"
#include <math.h>
#include "global.h"
#include "matrix.h"


void quat_mul(VEC *q1, VEC *q2, VEC *res)
{
    int i;
    VEC *temp;
    temp = v_get(4);
    
    if(q1->dim != 4 || q2->dim != 4)
    {
       // cliPrintf("\nquaternion dimensions not correct in quat_mul");
    }
    
	temp->ve[3] = (q1->ve[3] * q2->ve[3]) - (q1->ve[0] * q2->ve[0]) - (q1->ve[1] * q2->ve[1]) - (q1->ve[2] * q2->ve[2]);
	temp->ve[0] = (q1->ve[3] * q2->ve[0]) + (q1->ve[0] * q2->ve[3]) + (q1->ve[1] * q2->ve[2]) - (q1->ve[2] * q2->ve[1]);
	temp->ve[1] = (q1->ve[3] * q2->ve[1]) + (q1->ve[1] * q2->ve[3]) + (q1->ve[2] * q2->ve[0]) - (q1->ve[0] * q2->ve[2]);
	temp->ve[2] = (q1->ve[3] * q2->ve[2]) + (q1->ve[2] * q2->ve[3]) + (q1->ve[0] * q2->ve[1]) - (q1->ve[1] * q2->ve[0]);
    
    for (i=0; i<4;i++){
         res->ve[i] = temp->ve[i];
    }
    
    v_free(temp);
        
}

void quat_mul_sc(VEC *q1, double s)
{
	q1->ve[0] = s*q1->ve[0];
	q1->ve[1] = s*q1->ve[1];
	q1->ve[2] = s*q1->ve[2];
	q1->ve[3] = s*q1->ve[3];
}


void quat_inv(VEC *q1, VEC *q2)
{
	q2->ve[0] = -q1->ve[0]; 
	q2->ve[1] = -q1->ve[1]; 
	q2->ve[2] = -q1->ve[2]; 
	q2->ve[3] =  q1->ve[3];
}


void quat_normalize(VEC *q1)
{
	double n = q1->ve[0]*q1->ve[0] + q1->ve[1]*q1->ve[1] + q1->ve[2]*q1->ve[2] + q1->ve[3]*q1->ve[3];

	if (n == 1){
		return;
    }
	else if(n == 0){
        return;
    }
    else{
	quat_mul_sc( q1, (1/sqrt(n)) );
    }

}

void vec2quat(VEC *v1, VEC *q1)
{
	q1->ve[0] = v1->ve[0];
	q1->ve[1] = v1->ve[1];
	q1->ve[2] = v1->ve[2];
	q1->ve[3] = 0;
}

void quat2vec(VEC *q1, VEC *v1)
{
	//if (q1->ve[3] == 0){
		v1->ve[0] = q1->ve[0];
		v1->ve[1] = q1->ve[1];
		v1->ve[2] = q1->ve[2];
	//	return 1;commented on 03/10/13 
	//}
	//else{
	//	return 0;
	//}
}

void quat_rot_vec(VEC *q1, VEC *v1){
	VEC *quat_of_vec, *qmulv, *qinv, *fin_rot_vec; //all are quaternions

    quat_of_vec = v_get(4);
    v_zero(quat_of_vec);
    qmulv = v_get(4);
    v_zero(qmulv);
    qinv = v_get(4);
    v_zero(qinv);
    fin_rot_vec = v_get(4);
    v_zero(fin_rot_vec);
	vec2quat(v1, quat_of_vec); 
	quat_inv(q1, qinv);
	quat_mul(qinv, quat_of_vec, qmulv);
	quat_mul(qmulv, q1, fin_rot_vec);
	quat2vec(fin_rot_vec, v1);
    
    v_free(quat_of_vec);
    v_free(qmulv);
    v_free(qinv);
    v_free(fin_rot_vec);
    
}


void get_dyad(VEC *v1, VEC* v2, MAT *m)
{
    int i,j;
    
    if(v1 == VNULL){
    	//cliPrintf("\nv1 is null in get_dyad");
    }
    if(v2 == VNULL){
    	//cliPrintf("\nv2 is null in get_dyad");
    }
    if(m == MNULL){
    	//cliPrintf("\nm is null in get_dyad but its fixed now");
        m = m_get(v1->dim, v1->dim);
    }
    
    if((m->m != v1->dim) && (m->n != v2->dim))
    {
        m = m_resize(m, v1->dim, v2->dim);
    }
    
    for(i=0; i<v1->dim; i++)
    {
        for(j=0; j<v2->dim; j++)
        {
            m->me[i][j] = v1->ve[i] * v2->ve[j];
        }
        
    }
}


