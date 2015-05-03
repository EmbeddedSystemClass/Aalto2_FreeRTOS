#include "matrix.h"

static Real em = 4.440892E-16;

int m_converged(MAT *M1, MAT *M2)
{
	int i, j;
	double d;
	Real norm = m_norm_inf(M1);

	for(i = 0; i < M1->n; i++)
	{
		for(j = 0; j < M1->m; j++)
		{
			d = fabs(M1->me[i][j] - M2->me[i][j]);
			if(d > norm*em*100000.0)
			{
				return 0;
			}
		}
	}
	return 1;
}

void sylv_core(MAT *A, MAT *B, MAT *C)
{
	int i, j, i2;
	int n = B->n;
	int m = A->n;
	MAT *K = m_get(C->m*2, C->n*2);

	int l, k, l2, k2;
	MAT *P = m_get(C->m, 2);
	
	MAT *Kr4 = m_get(4, 4);
	MAT *Kr2;
	Kr2 = NEW(MAT);
	Kr2->me = NEW_A(2, Real *);
	Kr2->m = 2;
	Kr2->max_m = 2;
    Kr2 = sub_mat(Kr4, 0, 0, 1, 1, Kr2);

	VEC *V4, *V2;
	V4 = v_get(4);
	V2 = NEW(VEC);
	V2 = sub_vec(V4, 0, 1, V2);

	VEC *diag2, *diag4;
	diag4 = v_get(4);
	diag2 = NEW(VEC);
	diag2 = sub_vec(diag4, 0, 1, diag2);


	l = 0;
	while(l < n)
	{
		k = 0;
		l2 = l;
		if(l < n-1 && B->me[l+1][l] != 0.0)
		{
			l2 = l+1;
		}
		if(l > 0)
		{
			m_zero(P);
			for(i = 0; i < C->m; i++)
			{
				for(j = l; j <= l2; j++)
				{
					for(i2 = 0; i2 <= l-1; i2++)
					{
						P->me[i][j-l] += C->me[i][i2]*B->me[i2][j];
					}
				}
			}
			for(i = 0; i < C->m; i++)
			{
				for(j = l; j <= l2; j++)
				{
					C->me[i][j] -= P->me[i][j-l];
				}
			}
		}
		while(k < m)
		{
			k2 = k;
			if(k < m-1 && A->me[k+1][k] != 0.0)
			{
				k2 = k+1;
			}
			if(k > 0)
			{
				m_zero(P);
				for(i = k; i <= k2; i++)
				{
					for(j = l; j <= l2; j++)
					{
						for(i2 = 0; i2 <= k-1; i2++)
						{
							P->me[i-k][j-l] += C->me[i2][j]*A->me[i2][i];
						}
					}
				}
				for(i = k; i <= k2; i++)
				{
					for(j = l; j <= l2; j++)
					{
						C->me[i][j] -= P->me[i-k][j-l];
					}
				}
			}

			m_zero(Kr4);
			if(l2-l+1 == 2 && k2-k+1 == 2)
			{
				for(i = 0; i < 4; i++)
				{
					for(j = 0; j < 4; j++)
					{
						Kr4->me[j][i] += A->me[k+i%2][k+j%2]*((i<2&&j<2)||(i>1&&j>1));
						Kr4->me[j][i] += B->me[l+((i-i%2)>>1)][l+((j-j%2)>>1)]*(i%2==j%2);
					}
					V4->ve[i] = C->me[k+i%2][l+((i-i%2)>>1)];
				}
				QRfactor(Kr4, diag4);
				QRsolve(Kr4, diag4, V4, V4);
				for(i = 0; i < 4; i++)
				{
					C->me[k+i%2][l+((i-i%2)>>1)] = V4->ve[i];
				}
			}
			else if(l2-l+1 == 2 && k2-k+1 == 1)
			{
				for(i = 0; i < 2; i++)
				{
					for(j = 0; j < 2; j++)
					{
						Kr4->me[j][i] += A->me[k][k]*(i%2==j%2);
						Kr4->me[j][i] += B->me[l+i][l+j];
					}
					V4->ve[i] = C->me[k][l+i%2];
				}
				QRfactor(Kr2, diag2);
				QRsolve(Kr2, diag2, V2, V2);
				for(i = 0; i < 2; i++)
				{
					C->me[k][l+i%2] = V2->ve[i];
				}
			}
			else if(l2-l+1 == 1 && k2-k+1 == 2)
			{
				for(i = 0; i < 2; i++)
				{
					for(j = 0; j < 2; j++)
					{
						Kr4->me[j][i] += A->me[k+i][k+j];
						Kr4->me[j][i] += B->me[l][l]*(i%2==j%2);
					}
					V4->ve[i] = C->me[k+i%2][l];
				}
				QRfactor(Kr2, diag2);
				QRsolve(Kr2, diag2, V2, V2);
				for(i = 0; i < 2; i++)
				{
					C->me[k+i%2][l] = V2->ve[i];
				}

			}
			else
			{
				Kr4->me[0][0] = A->me[k][k]+B->me[l][l];
				V4->ve[0] = C->me[k][l];
				C->me[k][l] = V4->ve[0]/Kr4->me[0][0];
			}

			k = k2+1;
		}
		l = l2+1;
	}
	m_free(K);
	m_free(P);
	m_free(Kr4);
	free(Kr2->me);
	free(Kr2);
	v_free(V4);
	free(V2);
	free(diag2);
	v_free(diag4);
}

void lyap2(MAT *Ai, MAT *Ci, MAT *C)
{
	int n = Ai->m;
	int k;
	int i, j, i2;
	int l, l2;
	MAT *A3 = m_get(3,3);
	MAT *As = NEW(MAT);
	MAT *Bs = NEW(MAT);
	MAT *Cs = NEW(MAT);
	VEC *diag = v_get(3);
	VEC *F = v_get(3);
	MAT *A = m_get(Ai->m, Ai->n);
	MAT *U = m_get(Ai->m, Ai->n);
	MAT *C2 = m_get(C->m, C->n);

	m_copy(Ai, A);
	schur(A, U);

	m_mlt(Ci, U, C2);
	m_transp(U, U);
	m_mlt(U, C2, C);


	l = 0;
	while(l < n)
	{
		k = l;
		if(n-l > 1 && A->me[l+1][l] != 0.0)
		{
			A3->me[0][0] = A->me[l][l];
			A3->me[0][1] = A->me[l+1][l];
			A3->me[0][2] = 0;
			A3->me[1][0] = A->me[l][l+1];
			A3->me[1][1] = A->me[l][l] + A->me[l+1][l+1];
			A3->me[1][2] = A->me[l+1][l];
			A3->me[2][0] = 0;
			A3->me[2][1] = A->me[l][l+1];
			A3->me[2][2] = A->me[l+1][l+1];
			F->ve[0] = C->me[l][l]/2;
			F->ve[1] = C->me[l][l+1];
			F->ve[2] = C->me[l+1][l+1]/2;

			QRfactor(A3, diag);
			QRsolve(A3, diag, F, F);

			C->me[l][l] = F->ve[0];
			C->me[l][l+1] = F->ve[1];
			C->me[l+1][l] = F->ve[1];
			C->me[l+1][l+1] = F->ve[2];
			l++;
		}
		else
		{
			C->me[l][l] = C->me[l][l]/(2*A->me[l][l]);
		}

		l2 = l+1;
		if(n > l2)
		{
			As->me = NEW_A(l-k+1,Real *);
			As->m = l-k+1;
			As->n = l-k+1;
			As->max_m = l-k+1;
			As->max_n = l-k+1;
			Bs->me = NEW_A(n-1-l2+1,Real *);
			Bs->m = n-1-l2+1;
			Bs->n = n-1-l2+1;
			Bs->max_m = n-1-l2+1;
			Bs->max_n = n-1-l2+1;
			Cs->me = NEW_A(l-k+1,Real *);
			Cs->m = l-k+1;
			Cs->n = n-1-l2+1;
			Cs->max_m = l-k+1;
			Cs->max_n = n-1-l2+1;

			As = sub_mat(A, k, k, l, l, As);
			Bs = sub_mat(A, l2, l2, n-1, n-1, Bs);
			Cs = sub_mat(C, k, l2, l, n-1, Cs);

			for(i = k; i < l2; i++)
			{
				for(j = l2; j < n; j++)
				{
					for(i2 = k; i2 < l2; i2++)
					{
						C->me[i][j] -= C->me[i][i2]*A->me[i2][j];
					}
				}
			}
			sylv_core(As, Bs, Cs);
			for(i = l2; i < n; i++)
			{
				for(j = k; j < l2; j++)
				{
					C->me[i][j] = C->me[j][i];
				}
			}
			free(As->me);
			free(Bs->me);
			free(Cs->me);

		}
		for(i = l2; i < n; i++)
		{
			for(j = l2; j < n; j++)
			{
				for(i2 = k; i2 < l2; i2++)
				{
					C->me[i][j] -= A->me[i2][i]*C->me[i2][j];
					C->me[i][j] -= C->me[i2][i]*A->me[i2][j];
				}
			}
		}
		l = l2;
	}
	m_mlt(C, U, C2);
	m_transp(U, U);
	m_mlt(U, C2, C);

	m_free(A3);
	free(As);
	free(Bs);
	free(Cs);
	m_free(A);
	m_free(U);
	m_free(C2);
	v_free(diag);
	v_free(F);

}

void kleinman(MAT *A, MAT *B, MAT *Q, MAT *R, MAT *X0, MAT *X, double *iter)
{
	/* corrupts X0 */
	int i;
	MAT *K = m_get(X0->m, R->m);
	MAT *Kt = m_get(R->m, X0->m);
	MAT *PB = m_get(X0->m, B->n);
	MAT *invR = m_get(R->n, R->m);
	MAT *Ak = m_get(A->m, A->n);
	MAT *D = m_get(Q->m, Q->n);
	MAT *T = m_get(Q->m, Q->n);
	m_inverse(R, invR);

	for(i = 0; i < 10; i++)
	{
		sm_mlt(-1, X0, X0);
		m_mlt(X0, B, PB);
		m_mlt(PB, invR, K);
		m_transp(K, Kt);

		m_mlt(K, R, T);
		m_mlt(T, Kt, D);
		m_add(Q, D, D);
		sm_mlt(-1, D, D);
		
		m_mlt(B, Kt, Ak);
		m_add(A, Ak, Ak);
		lyap2(Ak, D, X);
	
		sm_mlt(-1, X0, X0);
		if(m_converged(X, X0))
		{
			i++;
			break;
		}
		
		m_copy(X, X0);

	}
	*iter = (double)i;

	m_free(K);
	m_free(Kt);
	m_free(PB);
	m_free(invR);
	m_free(Ak);
	m_free(D);
	m_free(T);
}


