#include <math.h>
#include "UKF.h"
#include "matrix.h"
#include "matrix2.h"
#include "quat.h"

#define v_resize(vec, size) v_resize_func(vec, size)

VEC *v_resize_func(VEC * vec, int size)
{
	V_FREE(vec);
 	vec = v_get(size);
 	return vec;
}


MAT *inertia = {MNULL};
FILE *fp=NULL, *fp1=NULL, *fp2=NULL;
int fo, fo1;



/************expandstate************************************************
 **********************************************************************/


void expandstate(MAT *q_x_err, VEC *x_full, MAT *q_x_expanded)
{
    VEC *q_temp, *q_err_vec, *q_err_quat;
    double temp, n_err_states, n_states;
    int i, j;

    n_states = 7+6;
    n_err_states = 6+6;
    
    q_temp = v_get(4);
    v_zero(q_temp);
   
    q_err_vec = v_get(3);

    
    q_err_quat = v_get(4);    
   
    for(j=0; j<n_err_states; j++)
    {
        for(i=0; i<3; i++)
        {
            q_err_vec->ve[i] = q_x_err->me[i][j];
        }   
        
        temp = v_norm2(q_err_vec);
            
        if(temp<=1 ){
					
			q_err_quat->ve[0] = q_err_vec->ve[0];
            q_err_quat->ve[1] = q_err_vec->ve[1];
            q_err_quat->ve[2] = q_err_vec->ve[2];
            q_err_quat->ve[3] = sqrt(1-temp); 
		}
		else//baad main daala hai
		{
			q_err_quat->ve[0] = (q_err_vec->ve[0])/(sqrt(1+temp));
            q_err_quat->ve[1] = (q_err_vec->ve[1])/(sqrt(1+temp));
            q_err_quat->ve[2] = (q_err_vec->ve[2])/(sqrt(1+temp));
            q_err_quat->ve[3] = 1/sqrt(1+temp);
		}

        quat_mul(q_err_quat, x_full, q_temp);
        for(i=0; i<4; i++)
        {
            q_x_expanded->me[i][j] = q_temp->ve[i]; 
        }
        for(i=4; i<7; i++)
        {
            q_x_expanded->me[i][j] = q_x_err->me[i-1][j] + x_full->ve[i];
        }
        for(i=7; i<10; i++)
        {
            q_x_expanded->me[i][j] = q_x_err->me[i-1][j] + x_full->ve[i];
        }
        for(i=10; i<13; i++)
        {
            q_x_expanded->me[i][j] = q_x_err->me[i-1][j] + x_full->ve[i];
        }
        
    }

// 
    v_free(q_temp);         
    v_free(q_err_vec);
    v_free(q_err_quat);
}




/*******************************RK4***********************************
 *********************************************************************/
void skew_mat(MAT *skew_mat, VEC *w)
{
    	
	if((skew_mat->m !=3) || (skew_mat->m !=4)){
		m_resize(skew_mat,4,4);
		m_zero(skew_mat);
	}
	if(skew_mat->m == 4){
        
        skew_mat->me[0][1] = w->ve[2]; 
        skew_mat->me[0][2] = (-1)*w->ve[1];
        skew_mat->me[0][3] = w->ve[0]; 
        skew_mat->me[1][0] = (-1)*w->ve[2];
		skew_mat->me[1][2] = w->ve[0];
        skew_mat->me[1][3] = w->ve[1]; 
        skew_mat->me[2][0] = w->ve[1]; 
		skew_mat->me[2][1] = (-1)*w->ve[0];
		skew_mat->me[2][3] = w->ve[2];         
        skew_mat->me[3][0] = (-1)*w->ve[0];        
		skew_mat->me[3][1] = (-1)*w->ve[1];
        skew_mat->me[3][2] = (-1)*w->ve[2];
    }
	else if(skew_mat->m == 3){// not needed
        
        skew_mat->me[0][1] = w->ve[2]; 
        skew_mat->me[0][2] = (-1)*w->ve[1];
        skew_mat->me[1][2] = w->ve[0]; 
        skew_mat->me[1][0] = (-1)*w->ve[2];
        skew_mat->me[2][0] = w->ve[1]; 
        skew_mat->me[2][1] = (-1)*w->ve[0];
    }
    
}


/*******************************RK4***********************************
 ********************************************************************/

void Sy_m( VEC *x, VEC *Torq_ext, VEC *out)
{
    static MAT *iI = {MNULL};
	MAT *sk_om = {MNULL};
    VEC *q, *w, *q_dot, *w_dot, *v_res1, *v_res2; 
    int i;

    if ( x == VNULL || out == VNULL ){
        error(E_NULL,"f");
	}
    if ( x->dim != 13 || out->dim != 13){
        error(E_SIZES,"f");
	}

 
    q = v_get(4);
    for (i=0; i<4; i++)
    {
        q->ve[i] = x->ve[i];
    }
    
    w = v_get(3);
    for (i=4; i<7; i++)
    {
        w->ve[i-4] = x->ve[i];
    }
               
    v_res1 = v_get(3);
    v_zero(v_res1);
    
    v_res2 = v_get(3);
    v_zero(v_res2);
    
    q_dot = v_get(4);
    v_zero(q_dot);
    
    w_dot = v_get(3);
    v_zero(w_dot);
    
    if(iI == MNULL){
        iI = m_get(3,3);
        m_zero(iI);
    }
    sk_om = m_get(4,4);
    skew_mat(sk_om, w);
           
    mv_mlt(sk_om, q, q_dot);
    sv_mlt(0.5, q_dot, q);
    
        
    for (i=0; i<4; i++)
    {
        out->ve[i] = q->ve[i];
    }
       
    /*****wd********/
    m_resize(sk_om, 3, 3);
    mv_mlt(inertia, w, v_res1);
    mv_mlt(sk_om, v_res1, v_res2);
    v_sub(Torq_ext, v_res2, v_res1);
    m_inverse(inertia,iI);
    mv_mlt(iI, v_res1, w_dot);    
        
    for (i=4; i<7; i++)
    {
        out->ve[i] = w_dot->ve[i-4];
    }
   
    
    for (i=7; i<13; i++)
    {
        out->ve[i] = 0;
    }
    
    
    M_FREE(sk_om);
    //M_FREE(iI);
    V_FREE(q);
    V_FREE(w);
    V_FREE(q_dot);
    V_FREE(w_dot);
    V_FREE(v_res1);
    V_FREE(v_res2);
    
      
}

/*******************************RK4***********************************
 *          rk4 
 *********************************************************************/

double rk4(double t, VEC *x, double h, VEC *Torq)
{
    VEC *v1=VNULL, *v2=VNULL, *v3=VNULL, *v4=VNULL;
    VEC *temp=VNULL;
    double step_size=0.5*h;

    if ( x == VNULL )
        error(E_NULL,"rk4");

    v1   = v_get(x->dim);
    v2   = v_get(x->dim);
    v3   = v_get(x->dim);
    v4   = v_get(x->dim);
    temp = v_get(x->dim);
   
    Sy_m(x, Torq, v1); 
    v_mltadd(x,v1,step_size,temp);    
    Sy_m(temp, Torq, v2); 
    v_mltadd(x,v2,step_size,temp);   
    Sy_m(temp, Torq, v3);
    step_size = h;
    v_mltadd(x, v3, step_size, temp);        
    Sy_m(temp, Torq, v4);
    temp = v_copy(v1,temp);           
    v_mltadd(temp,v2,2.0,temp); 
    v_mltadd(temp,v3,2.0,temp); 
    v_add(temp,v4,temp);
    v_mltadd(x,temp,(h/6.0),x);   
     return t+h;   
     
    V_FREE(v1);
    V_FREE(v2);
    V_FREE(v3);
    V_FREE(v4);
    V_FREE(temp);
    
}

/**********************Normalize vector*******************************
 ********************************************************************/
void normalize_vec(VEC *vec)
{
    double norm;
    norm = v_norm2(vec);
    if(norm!=0)
    {
        sv_mlt((1/norm), vec, vec);
    }
    
}

/************************if_equal**********************************
 *check if two input vecs are equal
 *********************************************************************/
int is_vec_equal(VEC *vec1, VEC *vec2)
{
    unsigned int i;
    i=0;
    if(vec1->dim == vec2->dim){
	
        while( (i < (vec1->dim) && (vec1->ve[i]==vec2->ve[i])) ){
			i = i+1; 
        }
		if(i == ( (vec1->dim)) ){
			return 1;
		}
		else{
			return 0;
		}

	}
    else{
        return 0;
    }
    
        
}


int openfile()
{
	/*if ((fp=fopen("C:\\Users\\aalto-1\\Documents\\MATLAB\\SatSimToolbox\\test\\ads\\covariance.mat","w")) != NULL)
	{
		m_save(fp, inertia, "inertia");
		fo = fclose(fp);
		return (1);
	}
	else 
	{
		return(0);
	}*/
}


printerr(double sim_time)
{
	/*if ((fp1 = fopen("\\\\home.org.aalto.fi\\khursho1\\data\\Documents\\MATLAB\\SatSimToolbox\\test\\ads\\cov.txt", "a")) != NULL)
	{
		fprintf(fp1, "problem in CHfactor at %Lf \n", sim_time);
		fclose(fp1);
	}
	*/
}

double rad2deg(double angle)
{
	angle = angle * (180/3.1416);
	return angle;

}


double deg2rad(double angle)
{
	angle = angle * (3.1416/180);
	return angle;
}


/***************Unscented Kalman filter*******************************
 *********************************************************************/

void Ukf(VEC *omega, VEC *mag_vec, VEC *mag_vec_I, VEC *sun_vec, VEC *sun_vec_I, VEC *Torq_ext, double t, double h, int eclipse, VEC *state, VEC *st_error, VEC *residual, int *P_flag, double sim_time)
{
    static VEC *omega_prev = VNULL, *mag_vec_prev = VNULL, *sun_vec_prev = VNULL, *q_s_c = VNULL, *x_prev = VNULL, *Torq_prev, *x_m_o;
    static MAT *Q = {MNULL}, *R = {MNULL}, *Pprev = {MNULL};
    static double alpha, kappa, lambda, sqrt_lambda, w_m_0, w_c_0, w_i, beta;
    static int n_states, n_sig_pts, n_err_states, iter_num, initialize=0;
    
    VEC *x = VNULL, *x_priori = VNULL,  *x_err_priori = VNULL,  *single_sig_pt = VNULL, *v_temp = VNULL, *q_err_quat = VNULL,
            *err_vec = VNULL, *v_temp2 = VNULL, *x_ang_vel = VNULL, *meas = VNULL, *meas_priori = VNULL,
            *v_temp3 = VNULL, *x_posteriori_err = VNULL, *x_b_m = VNULL, *x_b_g = VNULL;
    MAT *sqrt_P = {MNULL}, *P = {MNULL}, *P_priori = {MNULL}, *sig_pt = {MNULL}, *sig_vec_mat = {MNULL},
            *err_sig_pt_mat = {MNULL}, *result = {MNULL}, *result_larger = {MNULL}, *result1 = {MNULL}, *Meas_err_mat = {MNULL},
            *P_zz = {MNULL}, *iP_vv = {MNULL}, *P_xz = {MNULL}, *K = {MNULL}, *result2 = {MNULL}, *result3 = {MNULL}, *C = {MNULL};
    
    int update_mag_vec, update_sun_vec, update_omega, i, j;
    double d_res;

    if (inertia == MNULL)
	{
		inertia = m_get(3,3);
		m_ident(inertia);
		inertia->me[0][0] = 0.007;
		inertia->me[1][1] = 0.014;
		inertia->me[2][2] = 0.015;
	}

    if (initialize == 0){
        iter_num = 1;
		n_states = (7+6);
        n_err_states = (6+6);
        n_sig_pts = 2*n_err_states+1;
        alpha = sqrt(3);
        kappa = 3 - n_states;
        lambda = alpha*alpha * (n_err_states+kappa) - n_err_states;
        beta = -(1-(alpha*alpha)); 
        w_m_0 = (lambda)/(n_err_states + lambda);
        w_c_0 = (lambda/(n_err_states + lambda)) + (1 - (alpha*alpha) + beta);
        w_i = 0.5/(n_err_states +lambda);
        initialize = 1;
        sqrt_lambda = (lambda+n_err_states);
        if(q_s_c == VNULL)
        {
            q_s_c = v_get(4);
            
            q_s_c->ve[0] = -0.020656;
            q_s_c->ve[1] = 0.71468;
            q_s_c->ve[2] = -0.007319;
            q_s_c->ve[3] = 0.6991;
        }
        if(Torq_prev == VNULL)
        {
            Torq_prev = v_get(3);
            v_zero(Torq_prev);
        }
        
        quat_normalize(q_s_c);
		
    }
      

    result = m_get(9,9);
    m_zero(result);
        
    result1 = m_get(n_err_states, n_err_states);
    m_zero(result1);
        
    if(x_m_o == VNULL)
	{
		x_m_o = v_get(n_states);
		v_zero(x_m_o);     
	}
	
	x = v_get(n_states);
    v_zero(x);
    
    
    x_err_priori = v_get(n_err_states);
    v_zero(x_err_priori);
    
    x_ang_vel = v_get(3);
    v_zero(x_ang_vel);
    
    sig_pt = m_get(n_states, n_err_states);
    m_zero(sig_pt);
    
    
	if (C == MNULL)
    {
        C = m_get(9, 12);
        m_zero(C);
    }    

    
    if (P_priori == MNULL)
    {
        P_priori = m_get(n_err_states, n_err_states);
        m_zero(P_priori);
    }
    
	
    if (Q == MNULL)
    {
        Q = m_get(n_err_states, n_err_states); 
        m_ident(Q);
        //
        Q->me[0][0] = 0.0001;
        Q->me[1][1] = 0.0001;
        Q->me[2][2] = 0.0001;
		
        Q->me[3][3] = 0.0001;
        Q->me[4][4] = 0.0001;
        Q->me[5][5] = 0.0001;

        Q->me[6][6] = 0.000001;
        Q->me[7][7] = 0.000001;
        Q->me[8][8] = 0.000001;

        Q->me[9][9]   = 0.000001;
        Q->me[10][10] = 0.000001;
        Q->me[11][11] = 0.000001;
	}

    

    if( Pprev == MNULL)
    {
        Pprev = m_get(n_err_states, n_err_states); 
        m_ident(Pprev);
		
        Pprev->me[0][0] = 1e-3;
        Pprev->me[1][1] = 1e-3;
        Pprev->me[2][2] = 1e-3;
        Pprev->me[3][3] = 1e-3;
        Pprev->me[4][4] = 1e-3;
        Pprev->me[5][5] = 1e-3;
        Pprev->me[6][6] = 1e-4;
        Pprev->me[7][7] = 1e-4;
        Pprev->me[8][8] = 1e-4;
        Pprev->me[9][9] =	1e-3;
        Pprev->me[10][10] = 1e-3;
        Pprev->me[11][11] = 1e-3;
    }



    if (R == MNULL)
    {
        R = m_get(9,9);
        m_ident(R);
    
        R->me[0][0] = 0.034;
        R->me[1][1] = 0.034;
        R->me[2][2] = 0.034;
        
        R->me[3][3] = 0.00027;
        R->me[4][4] = 0.00027;
        R->me[5][5] = 0.00027;
        
        R->me[6][6] = 0.000012;
        R->me[7][7] = 0.000012;
        R->me[8][8] = 0.000012;
    }

	if(eclipse==0)
	{
		R->me[0][0] = 0.00034;
        R->me[1][1] = 0.00034;
        R->me[2][2] = 0.00034;
        
        R->me[3][3] = 0.00027;
        R->me[4][4] = 0.00027;
        R->me[5][5] = 0.00027;
        
        R->me[6][6] = 0.0000012;
        R->me[7][7] = 0.0000012;
        R->me[8][8] = 0.0000012;


		Q->me[0][0] =	0.00001;
        Q->me[1][1] =	0.00001;
        Q->me[2][2] =	0.00001;

        Q->me[3][3] =	0.0001;//0.000012;//0.0175;//1e-3; 
        Q->me[4][4] =	0.0001;//0.0175;//1e-3;
        Q->me[5][5] =	0.0001;//0.0175;//1e-3;

        Q->me[6][6] =	0.0000000001;//1e-6;
        Q->me[7][7] =	0.0000000001;
        Q->me[8][8] =	0.0000000001;

        Q->me[9][9]   =	0.0000000001;
        Q->me[10][10] = 0.0000000001;
        Q->me[11][11] = 0.0000000001;
	}    
	else
	{
		R->me[0][0] = 0.34;
        R->me[1][1] = 0.34;
        R->me[2][2] = 0.34;

        R->me[3][3] =	0.0027;
        R->me[4][4] =	0.0027;
        R->me[5][5] =	0.0027;
        
        R->me[6][6] =	0.0000012;
        R->me[7][7] =	0.0000012;
        R->me[8][8] =	0.0000012;


		Q->me[0][0] =	0.00001;
        Q->me[1][1] =	0.00001;
        Q->me[2][2] =	0.00001;
		
        Q->me[3][3] =	0.0001;
        Q->me[4][4] =	0.0001;
        Q->me[5][5] =	0.0001;

        Q->me[6][6] =	0.0000000001;
        Q->me[7][7] =	0.0000000001;
        Q->me[8][8] =	0.0000000001;

        Q->me[9][9]   = 0.0000000001;
        Q->me[10][10] = 0.0000000001;
        Q->me[11][11] = 0.0000000001;
	}
    
    if(omega_prev == VNULL)
    {
        omega_prev = v_get(3);
        v_zero(omega_prev);
        
    }
    
    if(mag_vec_prev == VNULL)
    {
        mag_vec_prev = v_get(3);
        v_zero(mag_vec_prev);     
    }
    
    if(sun_vec_prev == VNULL)
    {
        sun_vec_prev = v_get(3);
        v_zero(sun_vec_prev);
    }
    
   
    if (err_sig_pt_mat == MNULL)
    {
        err_sig_pt_mat = m_get(n_err_states, n_sig_pts); 
        m_zero(err_sig_pt_mat);        
    }
    
    
    if(q_err_quat == VNULL)
    {
        q_err_quat = v_get(4);
//         q_err_quat = v_resize(q_err_quat,4);
        v_zero(q_err_quat);
    }
    
    if(err_vec == VNULL)
    {
        err_vec = v_get(3);
        v_zero(err_vec);
    }
    
    
    v_temp = v_get(9);
    
    v_resize(v_temp,3);

     
    if(x_prev == VNULL)
    {
        x_prev = v_get(n_states);
        v_zero(x_prev);
        x_prev->ve[3] = 1;
        
        quat_mul(x_prev,q_s_c,x_prev);
        
        x_prev->ve[4] = 0.0;
        x_prev->ve[5] = 0.0;
        x_prev->ve[6] = 0.0;
        
        x_prev->ve[7] = 0.0;
        x_prev->ve[8] = 0.0;
        x_prev->ve[9] = 0.0;
        
        x_prev->ve[10] = 0.0;
        x_prev->ve[11] = 0.0;
        x_prev->ve[12] = 0.0;
    }


    
    sqrt_P = m_get(n_err_states, n_err_states);
    m_zero(sqrt_P);


    //result = m_resize(result, n_err_states, n_err_states);
    result_larger = m_get(n_err_states, n_err_states);
    int n, m;
    for(n = 0; n < result->n; n++)
    {
    	for(m = 0; m < result->m; m++)
		{
			result_larger->me[m][n] = result->me[m][n];
		}
    }
    


	
	
 	//v_resize(v_temp, n_err_states);
 	V_FREE(v_temp);
 	v_temp = v_get(n_err_states);

	symmeig(Pprev, result_larger, v_temp);

	i = 0;
	for (j=0;j<n_err_states;j++){
		if(v_temp->ve[j]>=0);
		else{
			i = 1;
		}
		
	}
		
	m_copy(Pprev, result1);
	sm_mlt(sqrt_lambda, result1, result_larger);
	catchall(CHfactor(result_larger), printerr(sim_time));
	
	
	for(i=0; i<n_err_states; i++){
		for(j=i+1; j<n_err_states; j++){
			result_larger->me[i][j] = 0;
		}
	}

	expandstate(result_larger, x_prev, sig_pt);

    sig_vec_mat = m_get(n_states, n_sig_pts);
    m_zero(sig_vec_mat);
    
    
    for(j = 0; j<(n_err_states+1); j++)
    {
        
        for(i = 0; i<n_states; i++)
        {
			if(j==0)
			{
				sig_vec_mat->me[i][j] = x_prev->ve[i];
			}
            else if(j>0) 
			{
				sig_vec_mat->me[i][j] = sig_pt->me[i][j-1];
			}
		}
	}
	
	sm_mlt(-1,result_larger,result_larger);
    
    expandstate(result_larger, x_prev, sig_pt);
    
	for(j = (n_err_states+1); j<n_sig_pts; j++)
    {
        for(i = 0; i<n_states; i++)
        {
			sig_vec_mat->me[i][j] = sig_pt->me[i][j-(n_err_states+1)];
	    }
    }

    single_sig_pt = v_get(n_states); 

    
    quat_rot_vec(q_s_c, Torq_ext);
    
               
    for(j=0; j<(n_sig_pts); j++)
    {   
        //v_temp = v_resize(v_temp,n_states);
        V_FREE(v_temp);
        v_temp = v_get(n_states);
        get_col(sig_vec_mat, j, single_sig_pt);
        v_copy(single_sig_pt, v_temp);
        rk4(t, v_temp, h, Torq_prev);
        set_col(sig_vec_mat, j, v_temp);

    }
    
    v_copy(Torq_ext, Torq_prev);
    
    x_priori = v_get(n_states);
    v_zero(x_priori);
    
    
    v_resize(v_temp,n_states);
    v_zero(v_temp);
    
    for(j=0; j<n_sig_pts; j++)
    {
        get_col( sig_vec_mat, j, v_temp);
        if(j == 0)
        {
            v_mltadd(x_priori, v_temp, w_m_0, x_priori);
        }
        else 
        {
            v_mltadd(x_priori, v_temp, w_i, x_priori);
        }
        
    }

    
    v_copy(x_priori, v_temp);

    v_resize(v_temp,4);
    quat_normalize(v_temp);//zaroori hai ye
	
	
    for(i=0; i<4; i++)
    {
        x_priori->ve[i] = v_temp->ve[i];
    }
   

    v_resize(v_temp, n_states);
    v_copy(x_priori, v_temp);
    
    v_resize(v_temp, 4);
    
    quat_inv(v_temp, v_temp);
        
    
    for(i=0; i<3; i++)
    {
        x_ang_vel->ve[i] = x_priori->ve[i+4];
    }
     
    
   
    x_b_m = v_get(3);
    v_zero(x_b_m);
    x_b_g = v_get(3);
    v_zero(x_b_g);
    /////////////////////////check it!!!!!!!! checked... doesnt change much the estimate
    for(i=0; i<3; i++)
    {
        x_b_m->ve[i] = x_priori->ve[i+7];
        x_b_g->ve[i] = x_priori->ve[i+10];
    }
    
    v_temp2 = v_get(n_states);
    v_zero(v_temp2);


    
    for(j=0; j<n_sig_pts; j++)
    {
        v_resize(v_temp2, n_states);
        get_col( sig_vec_mat, j, v_temp2);

        for(i=0; i<3; i++)
        {
            err_vec->ve[i] = v_temp2->ve[i+4];
        }
        
        v_resize(v_temp2, 4);
        quat_mul(v_temp2, v_temp, q_err_quat);

        v_resize(q_err_quat, n_err_states);
        
        v_sub(err_vec, x_ang_vel, err_vec);
        for(i=3; i<6; i++)
        {
            q_err_quat->ve[i] = err_vec->ve[i-3];
        }
        
        for(i=0; i<3; i++)
        {
            err_vec->ve[i] = v_temp2->ve[i+7];
        }
        v_sub(err_vec, x_b_m, err_vec);
        for(i=6; i<9; i++)
        {
            q_err_quat->ve[i] = err_vec->ve[i-6];
        }
        
        for(i=0; i<3; i++)
        {
            err_vec->ve[i] = v_temp2->ve[i+10];
        }
        v_sub(err_vec, x_b_g, err_vec);
        for(i=9; i<12; i++)
        {
            q_err_quat->ve[i] = err_vec->ve[i-9];
        }
        
                
        set_col(err_sig_pt_mat, j, q_err_quat); 

        if(j==0){
            v_mltadd(x_err_priori, q_err_quat, w_m_0, x_err_priori);  
        }
        else{
            v_mltadd(x_err_priori, q_err_quat, w_i, x_err_priori);     
        }

    }
    
    v_resize(v_temp,n_err_states);
    for (j=0;j<13;j++)
    {
        get_col(err_sig_pt_mat, j, v_temp);
        v_sub(v_temp, x_err_priori, v_temp);
        get_dyad(v_temp, v_temp, result_larger);
        
        if(j==0){
            sm_mlt(w_c_0, result_larger, result_larger);
        }
        else{
            sm_mlt(w_i, result_larger, result_larger);
        }
        m_add(P_priori, result_larger, P_priori);
    }
    

	symmeig(P_priori, result_larger, v_temp);

	i = 0;
	for (j=0;j<n_err_states;j++){
		if(v_temp->ve[j]>=0);
		else{
			i = 1;
		}
		
	}


	m_add(P_priori, Q, P_priori);
	
	

   v_resize(v_temp,3);    
  
   meas = v_get(9);
   if (!(is_vec_equal(sun_vec, sun_vec_prev)) /*&& (eclipse==0)*/ ){
        update_sun_vec =1;
        v_copy(sun_vec, sun_vec_prev);
        v_copy(sun_vec, v_temp);
    
        normalize_vec(v_temp);
        quat_rot_vec(q_s_c, v_temp);  
        normalize_vec(v_temp);
        
        
        for(i = 0; i<3;i++){
            meas->ve[i] = v_temp->ve[i];
        }
    }
   else{
       update_sun_vec =0;
       for(i = 0; i<3;i++){
            meas->ve[i] = 0;
        }
    }
   
    
    if (!(is_vec_equal(mag_vec, mag_vec_prev)) ){
        update_mag_vec =1;
        v_copy(mag_vec, mag_vec_prev);
        v_copy(mag_vec, v_temp);
              
        normalize_vec(v_temp);
        quat_rot_vec(q_s_c, v_temp);
        normalize_vec(v_temp); 
        for(i=3; i<6; i++){
            meas->ve[i] = v_temp->ve[i-3];
        }
    }
    else{
        update_mag_vec =0;
        for(i=3; i<6; i++){
            meas->ve[i] = 0;//mag_vec_prev->ve[i-3];
        }
    }
     
    if (!(is_vec_equal(omega, omega_prev) ) ){
        update_omega =1;
        v_copy(omega, omega_prev);
        v_copy(omega, v_temp);
        
      
        quat_rot_vec(q_s_c, v_temp);
        for(i=6; i<9; i++){
            meas->ve[i] = v_temp->ve[i-6];
        }
    }
    else{
        update_omega =0;
        for(i=6; i<9; i++){
            meas->ve[i] = 0;
        }
    }    
    

    v_resize(v_temp, 9);
    v_resize(v_temp2, n_states);
    v_temp3 = v_get(3);
    
    Meas_err_mat = m_get(9, n_sig_pts);
    m_zero(Meas_err_mat);
    
    meas_priori = v_get(9);
    v_zero(meas_priori);
    
	
	    
    for(j=0; j<n_sig_pts; j++)
    {
        get_col( sig_vec_mat, j, v_temp2);
        
        if(update_omega){
           
            for(i=6;i<9;i++){
                v_temp->ve[i] = v_temp2->ve[i-2] + x_b_g->ve[i-6];
                
            }
        }
        else{
            for(i=6;i<9;i++){
                v_temp->ve[i] = 0;
            }
        }

        v_resize(v_temp2, 4); 

        if(update_sun_vec){
            for(i=0;i<3;i++){
                v_temp3->ve[i] = sun_vec_I->ve[i];
            }
            quat_rot_vec(v_temp2, v_temp3);
            normalize_vec(v_temp3);
            
            for(i=0;i<3;i++){
                v_temp->ve[i] = v_temp3->ve[i]; 
            }
			
			
        }
        else{
            for(i=0;i<3;i++){
                v_temp->ve[i] = 0;
            }
        }
        if(update_mag_vec){
            for(i=0;i<3;i++){
                v_temp3->ve[i] = mag_vec_I->ve[i];
            }
            normalize_vec(v_temp3);
            for(i=0;i<3;i++){
                v_temp3->ve[i] = v_temp3->ve[i] + x_b_m->ve[i];
            } 
            quat_rot_vec(v_temp2, v_temp3);
            normalize_vec(v_temp3);
           
            for(i=3;i<6;i++){
                v_temp->ve[i] = v_temp3->ve[i-3];
            }

			           
        }
        else{
            for(i=3;i<6;i++){
                v_temp->ve[i] = 0;
            }
        }
        
   
        set_col(Meas_err_mat, j, v_temp); 
        
        if(j==0){
            v_mltadd(meas_priori, v_temp, w_m_0, meas_priori);
        }
        else{
            v_mltadd(meas_priori, v_temp, w_i, meas_priori);  
        }
    }
	
	

	
	v_resize(v_temp, 9);

    m_resize(result_larger, 9, 9);
    m_zero(result_larger);
    
    P_zz = m_get(9, 9);
    m_zero(P_zz);
    
    iP_vv = m_get(9, 9);
    m_zero(iP_vv);
    
   
    P_xz = m_get(n_err_states, 9);
    m_zero(P_xz);
    
    v_resize(v_temp2, n_err_states);
    
    result1 = m_resize(result1,n_err_states,9);    
    
	for (j=0; j<n_sig_pts; j++)
    {
        get_col( Meas_err_mat, j, v_temp);
        
        get_col( err_sig_pt_mat, j, v_temp2);
        
	
        v_sub(v_temp, meas_priori, v_temp); 
        
        get_dyad(v_temp, v_temp, result_larger);
        
        get_dyad(v_temp2, v_temp, result1);
               
        if(j==0){
            sm_mlt(w_c_0, result_larger, result_larger);
            sm_mlt(w_c_0, result1, result1);
        }
        else{
            sm_mlt(w_i, result_larger, result_larger);
            sm_mlt(w_i, result1, result1);
        }
      
			
		m_add(P_zz, result_larger, P_zz);
        m_add(P_xz, result1, P_xz);
        
    }
	




	symmeig(P_zz, result_larger, v_temp);

	i = 0;
	for (j=0; j<9; j++){
		if(v_temp->ve[j]>=0);
		else{
			i = 1;
		}
	}


	m_add(P_zz, R, P_zz);
	
	m_inverse(P_zz, iP_vv);

	
    K = m_get(n_err_states, 9);
    m_zero(K);

    m_mlt(P_xz, iP_vv, K); 
	
	

    
    if(x_posteriori_err == VNULL)
    {
        x_posteriori_err = v_get(n_err_states);
        v_zero(x_posteriori_err);
    }
    v_resize(v_temp,9);
    
    v_sub(meas, meas_priori, v_temp);
    
    v_copy(v_temp, residual);
    mv_mlt(K, v_temp, x_posteriori_err);
     
    v_resize(v_temp2,3);
    for(i=0;i<3;i++){
        v_temp2->ve[i] = x_posteriori_err->ve[i];
    }
    
    
    for(i=4; i<n_states; i++){
       
        x_prev->ve[i] = (x_posteriori_err->ve[i-1] + x_priori->ve[i]);
    }
    
     
    
    d_res = v_norm2(v_temp2);
    v_resize(v_temp2,4);
	

	
    if(d_res<=1 /*&& d_res!=0*/){


        v_temp2->ve[0] = v_temp2->ve[0];
        v_temp2->ve[1] = v_temp2->ve[1];
        v_temp2->ve[2] = v_temp2->ve[2];
        v_temp2->ve[3] = sqrt(1-d_res); 

    }
	else//baad main daala hai
	{
		v_temp2->ve[0] = (v_temp2->ve[0])/(sqrt(1+d_res));
        v_temp2->ve[1] = (v_temp2->ve[1])/(sqrt(1+d_res));
        v_temp2->ve[2] = (v_temp2->ve[2])/(sqrt(1+d_res));
        v_temp2->ve[3] = 1/sqrt(1 + d_res);
	}
    
    v_resize(x_posteriori_err, n_states);

    for(i=(n_states-1); i>3; i--){
        x_posteriori_err->ve[i] = x_posteriori_err->ve[i-1];
    }
    for(i=0; i<4; i++){
        x_posteriori_err->ve[i] = v_temp2->ve[i];
    }

    
    quat_mul(v_temp2, x_priori, v_temp2);
   
    for(i=0;i<4;i++){
        x_prev->ve[i] = v_temp2->ve[i];
    }
   
     m_resize(result_larger, n_err_states, 9);
       
     m_mlt(K, P_zz, result_larger);
     result2 = m_get(9, n_err_states);
     
	m_transp(K,result2);
  
		
     m_resize(result1, n_err_states, n_err_states);
     m_mlt(result_larger, result2,  result1);
     v_resize(v_temp, n_err_states);
	
	 
	 m_sub(P_priori, result1, Pprev);

	 symmeig(Pprev, result1 , v_temp);

	 i = 0;
	 
     for (j=0;j<n_err_states;j++){
		 if(v_temp->ve[j]>=0);
		 else{
			 i = 1;
		 }
     }


    
	v_copy(x_prev, v_temp);
	v_resize(v_temp,4);
	v_copy(x_prev, v_temp2);
	v_resize(v_temp2,4);

	
	v_copy(x_prev, x_m_o);
	//v_resize(x_m_o, 4);

     v_resize(v_temp,3);
     quat_inv(q_s_c, v_temp2);
     v_copy( x_prev, state); 
     quat_mul(state, v_temp2, state);
		


     for(i=0; i<3; i++){
         v_temp->ve[i] = state->ve[i+4];
     }
     quat_rot_vec(v_temp2, v_temp);
     
     for(i=0; i<3; i++){
         state->ve[i+4] = v_temp->ve[i];
     }
     
    v_copy( x_posteriori_err, st_error);
    

		

    iter_num++;
    
	V_FREE(x);
	V_FREE(x_priori);
	V_FREE(x_err_priori);
	V_FREE(single_sig_pt);
	V_FREE(v_temp);
	V_FREE(q_err_quat);
	V_FREE(err_vec);
	V_FREE(v_temp2);
	V_FREE(x_ang_vel);
	V_FREE(meas);
	V_FREE(meas_priori);
	V_FREE(v_temp3);
	V_FREE(x_posteriori_err);
	V_FREE(x_b_m);
	V_FREE(x_b_g);
	
 
	M_FREE(sqrt_P);
	M_FREE(P);
	M_FREE(P_priori);
	M_FREE(sig_pt);
	M_FREE(sig_vec_mat);
	M_FREE(err_sig_pt_mat);
	M_FREE(result);
	M_FREE(result_larger);
	M_FREE(result1);
	M_FREE(Meas_err_mat);
	M_FREE(P_zz);
	M_FREE(iP_vv);
	M_FREE(P_xz);
	M_FREE(K);
	M_FREE(result2);
	M_FREE(result3);
     
}

