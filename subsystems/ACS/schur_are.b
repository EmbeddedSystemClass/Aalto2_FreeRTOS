#include "matrix.h"
#include "matrix2.h"
#include <math.h>

#define	sign(x)	((x) > 0.0 ? 1 : ((x) < 0.0 ? -1 : 0 ))

static Real ko = 1E-37;
static Real em = 4.440892E-16;

int same_elements(VEC *v1, VEC *v2)
{
	int i, j;
	int n = v1->dim;
	int num_eq = 0;
	for(i = 0; i < n; i++)
	{
		if(is_in(v1->ve[i], v2))
		{
			num_eq++;
		}
	}
	
	return (num_eq == n);
}

int is_in(Real r, VEC *v)
{
	int i;
	for(i = 0; i < v->dim; i++)
	{
		/*printf("\n_______________________________\n");
		printf("%.20f, %.20f, %.20f\n", v->ve[i], r, fabs(r)*em);
		printf("_______________________________\n");*/
		if(fabs(v->ve[i] - r) < fabs(r)*em*1000000.0)
		{
			/*printf("IN:         %f,   %f\n\n", v->ve[i], r);*/
			return 1;
		}
	}
	/*printf("OUT:         %f\n\n", r);*/
	return 0;
}

void indexed_mult(MAT *M, MAT *G, int s, int e, int i, int l, int flag)
{
	int *n, *m;
	VEC *in = v_get(l);
	VEC *out = v_get(l);
	int a, b, k;
	MAT*(*mlt)(MAT*, VEC*, VEC*);

	if(flag)
	{
		n = &a;
		m = &b;
		mlt = &vm_mlt;
	}
	else
	{
		m = &a;
		n = &b;
		mlt = &mv_mlt;
	}

	for((*n) = s; (*n) < e; (*n)++)
	{
		for(k = 0; k < l; k++)
		{
			(*m) = i+k;
			in->ve[k] = M->me[a][b];
		}
		(*mlt)(G, in, out);
		for(k = 0; k < l; k++)
		{
			(*m) = i+k;
			M->me[a][b] = out->ve[k];
		}
	}

	v_free(in);
	v_free(out);
}

void SylvCompleteP(MAT *A, MAT *B, MAT *C, MAT *X, Real *s)
{
	Real t;
	MAT *S;
	VEC *c;
	PERM *rows;
	PERM *cols;
	VEC *x;
	int i, j, k, l;
	Real max;
	int max_r, max_c;
	int tmp;
	Real s_min;
	Real scale;

	max = 0.0;
	for(k = 0; k < A->n; k++)
	{
		for(l = 0; l < A->m; l++)
		{
			if(fabs(A->me[l][k]) > max)
			{
				max = fabs(A->me[l][k]);
			}
		}
	}
	max = max*em;
	if(max < ko/em)
	{
		s_min = ko/em;
	}
	else
	{
		s_min = max;
	}
	max = 0.0;
	for(k = 0; k < B->n; k++)
	{
		for(l = 0; l < B->m; l++)
		{
			if(fabs(B->me[k][l]) > max)
			{
				max = fabs(B->me[k][l]);
			}
		}
	}
	max = max*em;
	if(s_min < max)
	{
		s_min = max;
	}


	if(A->n == 1 && B->n == 1)
	{
		X->n = 1;
		X->m = 1;
		t = A->me[0][0] - B->me[0][0];
		if(fabs(t) <= ko/em)
		{
			t = ko/em;
		}
		if(fabs(C->me[0][0]*ko/em > fabs(t)))
		{
			*s = 1/fabs(C->me[0][0]);
		}
		else
		{
			*s = 1.0;
		}
		
		X->me[0][0] = (*s)*C->me[0][0]/t;
		goto end;
	}
	else if(A->n == 1 && B->n == 2)
	{
		S = m_get(2, 2);
		c = v_get(2);
		rows = px_get(2);
		cols = px_get(2);
		x = v_get(2);
		X->n = 2;
		X->m = 1;
		S->me[0][0] = A->me[0][0] - B->me[0][0];
		S->me[0][1] = -B->me[1][0];
		S->me[1][0] = -B->me[0][1];
		S->me[1][1] = A->me[0][0] - B->me[1][1];
		c->ve[0] = C->me[0][0];
		c->ve[1] = C->me[0][1];
	}
	else if(A->n == 2 && B->n == 1)
	{
		S = m_get(2, 2);
		c = v_get(2);
		rows = px_get(2);
		cols = px_get(2);
		x = v_get(2);
		X->n = 1;
		X->m = 2;
		c->ve[0] = C->me[0][0];
		c->ve[1] = C->me[1][0];
		S->me[0][0] = A->me[0][0] - B->me[0][0];
		S->me[0][1] = A->me[0][1];
		S->me[1][0] = A->me[1][0];
		S->me[1][1] = A->me[1][1] - B->me[0][0];
	}
	else if(A->n == 2 && B->n == 2)
	{
		S = m_get(4, 4);
		c = v_get(4);
		rows = px_get(4);
		cols = px_get(4);
		x = v_get(4);
		X->n = 2;
		X->m = 2;
		S->me[0][0] = A->me[0][0] - B->me[0][0];
		S->me[0][1] = A->me[0][1];
		S->me[0][2] = -B->me[1][0];
		S->me[0][3] = 0.0;
		S->me[1][0] = A->me[1][0];
		S->me[1][1] = A->me[1][1] - B->me[0][0];
		S->me[1][2] = 0.0;
		S->me[1][3] = -B->me[1][0];
		S->me[2][0] = -B->me[0][1];
		S->me[2][1] = 0.0;
		S->me[2][2] = A->me[0][0] - B->me[1][1];
		S->me[2][3] = A->me[0][1];
		S->me[3][0] = 0.0;
		S->me[3][1] = -B->me[0][1];
		S->me[3][2] = A->me[1][0];
		S->me[3][3] = A->me[1][1] - B->me[1][1];
	
		c->ve[0] = C->me[0][0];
		c->ve[1] = C->me[1][0];
		c->ve[2] = C->me[0][1];
		c->ve[3] = C->me[1][1];
	}
	for(i = 0; i < S->n-1; i++)
	{
		max = 0.0;
		for(k = i; k < S->n; k++)
		{
			for(l = i; l < S->m; l++)
			{
				if(fabs(S->me[rows->pe[l]][cols->pe[k]]) > fabs(max))
				{
					max = S->me[rows->pe[l]][cols->pe[k]];
					max_r = l;
					max_c = k;
				}
			}
		}
		tmp = rows->pe[i];
		rows->pe[i] = rows->pe[max_r];
		rows->pe[max_r] = tmp;
		tmp = cols->pe[i];
		cols->pe[i] = cols->pe[max_c];
		cols->pe[max_c] = tmp;

		if(fabs(S->me[rows->pe[i]][cols->pe[i]]) < s_min)
		{
			S->me[rows->pe[i]][cols->pe[i]] = s_min;
		}

		for(k = i+1; k < S->m; k++)
		{
			scale = S->me[rows->pe[k]][cols->pe[i]]/S->me[rows->pe[i]][cols->pe[i]];
			for(l = i; l < S->m; l++)
			{
				S->me[rows->pe[k]][cols->pe[l]] = S->me[rows->pe[k]][cols->pe[l]]
					-scale*S->me[rows->pe[i]][cols->pe[l]];
			}
			c->ve[rows->pe[k]] = c->ve[rows->pe[k]] - scale*c->ve[rows->pe[i]];
		}
	}
	if(A->n + B->n == 2)
	{
		scale = 1.0;
		/*printf("scale  1\n");*/
	}
	else if(A->n + B->n == 3)
	{
		scale = 2.0;
		/*printf("scale  2\n");*/
	}
	else if(A->n + B->n == 4)
	{
		scale = 8.0;
		/*printf("scale  8\n");*/
	}

	*s = 1.0;
	max = 0.0;
	for(i = 0; i < c->dim; i++)
	{
		if(fabs(c->ve[i]) > max)
		{
			max = fabs(c->ve[i]);
		}
	}
	for(i = 0; i < S->n; i++)
	{
		if(fabs(S->me[rows->pe[i]][cols->pe[i]]) < scale*ko*c->ve[i])
		{
			*s = 1.0/(scale*max);
			printf("SCALE: %.20f\n", *s);
			break;
		}
	}

	for(i = S->m-1; i >= 0; i--)
	{
		x->ve[cols->pe[i]] = c->ve[rows->pe[i]];
		for(j = S->n-1; j > i; j--)
		{
			x->ve[cols->pe[i]] = x->ve[cols->pe[i]]-x->ve[cols->pe[j]]*S->me[rows->pe[i]][cols->pe[j]];
		}
		x->ve[cols->pe[i]] = x->ve[cols->pe[i]]/S->me[rows->pe[i]][cols->pe[i]];
	}
	/*for(i = 0; i < X->n; i++)
	{
		for(j = 0; j < X->m; j++)
		{
			X->me[j][i] = x->ve[j+i*X->n];
		}
	}*/
	if(X->m == 1 && X->n == 2)
	{
		X->me[0][0] = x->ve[0];
		X->me[0][1] = x->ve[1];
	}
	else if(X->m == 2 && X->n == 1)
	{
		X->me[0][0] = x->ve[0];
		X->me[1][0] = x->ve[1];
	}
	else if(X->m == 2 && X->n == 2)
	{
		X->me[0][0] = x->ve[0];
		X->me[1][0] = x->ve[1];
		X->me[0][1] = x->ve[2];
		X->me[1][1] = x->ve[3];
	}
end:
	/*px_output(rows);
	px_output(cols);*/
	m_free(S);
	v_free(c);
	px_free(rows);
	px_free(cols);
	v_free(x);

	/*m_output(A);
	m_output(B);
	m_output(C);
	m_output(X);*/

	/*printf("%f, %f, %f, %f\n", X->me[0][0], X->me[0][1], X->me[1][0], X->me[1][1]);*/
}

void rot(MAT *M, Real *c, Real *s)
{
	Real tau;
	Real off;
	Real v1, v2;
	Real m;

	*c = 1.0;
	*s = 0.0;
	if(M->me[0][0] != M->me[1][1])
	{
		tau = (M->me[0][1] + M->me[1][0])/(M->me[0][0] - M->me[1][1]);
		off = sqrt(tau*tau+1);
		v1 = tau - off;
		v2 = tau + off;

		m = (fabs(v1)<fabs(v2)) ? v1 : v2;
		
		*c = 1.0/(sqrt(1+m*m));
		*s = m*(*c);
	}
}

/*void schur2(MAT *M, Real *c, Real *s)
{
	Real tmp;
	Real t, p, sigma, tau;
	Real a, b, g, d;

	a = M->me[0][0];
	b = M->me[0][1];
	g = M->me[1][0];
	d = M->me[1][1];

	if(g == 0.0)
	{
		*c = 1.0;
		*s = 0.0;
		return;
	}
	if(b == 0.0)
	{
		*c = 0.0;
		*s = 1.0;
		tmp = M->me[1][1];
		M->me[1][1] = M->me[0][0];
		M->me[0][0] = tmp;
		M->me[0][1] = -M->me[1][0];
		M->me[1][0] = 0;
		return;
	}
	if(a == d && b*g < 0)
	{
		*c = 1.0;
		*s = 0.0;
		return;
	}

	t = a - d;
	p = t/2.0;
	sigma = b + g;
	tau = sqrt(sigma*sigma + t*t);
	*c = sqrt((1.0+fabs(sigma)/tau)/2);
	*s = -sign(sigma)*p/(tau*(*c));

	M->me[0][0] = (*c)*(*c)*a + (*c)*(*s)*(g + b) + (*s)*(*s)*d;
	M->me[0][1] = (*c)*(*c)*b + (*c)*(*s)*(d-a) - (*s)*(*s)*g;
	M->me[1][0] = (*c)*(*c)*g + (*c)*(*s)*(d-a) - (*s)*(*s)*b;
	M->me[1][1] = (*c)*(*c)*d - (*c)*(*s)*(g+b) + (*s)*(*s)*a;

	t = (M->me[0][0] + M->me[1][1])/2.0;
	M->me[0][0] = t;
	M->me[1][1] = t;

	if(M->me[1][0] != 0.0)
	{
		if(M->me[0][1] != 0.0)
		{
			if(M->me[0][1]*M->me[1][0] > 0)
			{
				p = sign(M->me[1][0])*sqrt(M->me[0][1]*M->me[1][0]);
				tau = 1/sqrt(fabs(M->me[0][1]+M->me[1][0]));
				a = sqrt(abs(M->me[0][1]))*tau;
				b = sqrt(abs(M->me[1][0]))*tau;
				M->me[0][0] = t+p;
				M->me[1][1] = t-p;
				M->me[0][1] = M->me[0][1] - M->me[1][0];
				M->me[1][0] = 0;
				t = (*c)*a-(*s)*b;
				*s = (*c)*b+(*s)*a;
				*c = t;
			}
		}
		else
		{
			M->me[0][1] = -M->me[1][0];
			M->me[1][0] = 0;
			t = *c;
			*c = -(*s);
			*s = t;
		}
	}

}*/

int ordschur(MAT *T, MAT *U)
{
	int n = T->n;
	int i;
	VEC *stable_diag;
	VEC *current_diag;
	Real c, s;
	Real t;
	MAT *G = m_get(2, 2);
	MAT *As = NEW(MAT);
	MAT *Bs = NEW(MAT);
	MAT *Cs = NEW(MAT);
	MAT *Ts = NEW(MAT);
	MAT *X = m_get(2, 2);
	Real ss;
	MAT *Q = m_get(4, 4);
	MAT *Qt = m_get(4, 4);
	MAT *H = m_get(4, 2);
	MAT *D = m_get(4, 4);
	MAT *D2 = m_get(4, 4);
	VEC *qrdiag = v_get(2);
	int decision;

	int subspace_size = 0;
	for(i = 0; i < n; i++)
	{
		if(T->me[i][i] < 0.0)
		{
			subspace_size++;
		}
	}
	stable_diag = v_get(subspace_size);
	current_diag = v_get(subspace_size);
	subspace_size = 0;
	for(i = 0; i < n; i++)
	{
		if(T->me[i][i] < 0.0)
		{
			stable_diag->ve[subspace_size] = T->me[i][i];
			subspace_size++;
		}
	}
	v_sort(stable_diag, NULL);
	for(i = 0; i < subspace_size; i++)
	{
		current_diag->ve[i] = T->me[i][i];
	}

	while(!same_elements(stable_diag, current_diag))
	{
		i = 0;
		while(i < n-1)
		{
			if(T->me[i+1][i] == 0.0) 
			{
				if(i == n-2)
				{
					decision = 1;
				}
				else if(T->me[i+2][i+1] == (double)0.0)
				{
					decision = 1;
				}
				else if(T->me[i+2][i+1] != (double)0.0)
				{
					decision = 2;
				}
				
				if(decision == 1)
				{
					if(!is_in(T->me[i][i], stable_diag) && is_in(T->me[i+1][i+1], stable_diag))
					{
						givens(T->me[i][i+1], T->me[i+1][i+1]-T->me[i][i], &c, &s);
						G->me[0][0] = -c;
						G->me[0][1] = -s;
						G->me[1][0] = s;
						G->me[1][1] = -c;

						indexed_mult(T, G, i, T->n, i, 2, 0);
						G->me[0][0] = -c;
						G->me[0][1] = s;
						G->me[1][0] = -s;
						G->me[1][1] = -c;
						indexed_mult(T, G, 0, i+2, i, 2, 1);
						indexed_mult(U, G, 0, U->m, i, 2, 1);

						T->me[i+1][i] = 0.0;
					}
					i = i+1;
				}
				else if(decision == 2)
				{
					if(!is_in(T->me[i][i], stable_diag) && is_in(T->me[i+1][i+1], stable_diag))
					{
						t = T->me[i][i];

						As->me = NEW_A(1 ,Real *);
						As = sub_mat(T, i, i, i, i, As);
						Bs->me = NEW_A(2 ,Real *);
						Bs = sub_mat(T, i+1, i+1, i+2, i+2, Bs);
						Cs->me = NEW_A(1 ,Real *);
						Cs = sub_mat(T, i, i+1, i, i+2, Cs);
						Ts->me = NEW_A(3 ,Real *);
						Ts = sub_mat(T, i, i, i+2, i+2, Ts);
						SylvCompleteP(As, Bs, Cs, X, &ss);

						H->me[0][0] = -X->me[0][0];
						H->me[0][1] = -X->me[0][1];
						H->me[1][0] = 1.0*ss;
						H->me[1][1] = 0.0;
						H->me[2][0] = 0.0;
						H->me[2][1] = 1.0*ss;
						H->n = 2;
						H->m = 3;
						QRfactor(H, qrdiag);
						Q->n = 3;
						Q->m = 3;
						D->n = 3;
						D->m = 3;
						D2->n = 3;
						D2->m = 3;
						makeQ(H, qrdiag, Q);
						m_transp(Q, Qt);
						m_copy(Ts, D);
						m_mlt(Qt, D, D2);
						m_mlt(D2, Q, D);
						free(As->me);
						free(Bs->me);
						free(Cs->me);
						free(Ts->me);

						if(D->me[2][0] > m_norm_inf(Ts)*em || D->me[2][1] > m_norm_inf(Ts)*em)
						{
							return -1;
						}
						indexed_mult(T, Qt, i, T->n, i, 3, 0);
						indexed_mult(T, Q, 0, i+3, i, 3, 1);
						T->me[i+2][i] = 0.0;
						T->me[i+2][i+1] = 0.0;
						T->me[i+2][i+2] = t;

						indexed_mult(U, Q, 0, U->m, i, 3, 1);

						Ts->me = NEW_A(2 ,Real *);
						Ts = sub_mat(T, i, i, i+1, i+1, Ts);
						rot(Ts, &c, &s);
						free(Ts->me);
						G->me[0][0] = c;
						G->me[0][1] = s;
						G->me[1][0] = -s;
						G->me[1][1] = c;
						indexed_mult(T, G, 0, T->n, i, 2, 0);
						G->me[0][0] = c;
						G->me[0][1] = -s;
						G->me[1][0] = s;
						G->me[1][1] = c;
						indexed_mult(T, G, 0, T->m, i, 2, 1);
						indexed_mult(U, G, 0, U->m, i, 2, 1);
					}
					i = i+1;
				}
			}
			else if(T->me[i+1][i] != 0.0)
			{
				if(i == n-3)
				{
					decision = 1;
				}
				else if(i == n-2)
				{
					decision = 0;
					i = i+2;
				}
				else if(T->me[i+3][i+2] == (double)0.0)
				{
					decision = 1;
				}
				else if(T->me[i+3][i+2] != (double)0.0)
				{
					decision = 2;
				}

				if(decision == 1)
				{
					if(!is_in(T->me[i][i], stable_diag) && is_in(T->me[i+2][i+2], stable_diag))
					{
						t = T->me[i+2][i+2];
						As->me = NEW_A(2 ,Real *);
						As = sub_mat(T, i, i, i+1, i+1, As);
						Bs->me = NEW_A(1 ,Real *);
						Bs = sub_mat(T, i+2, i+2, i+2, i+2, Bs);
						Cs->me = NEW_A(2 ,Real *);
						Cs = sub_mat(T, i, i+2, i+1, i+2, Cs);
						Ts->me = NEW_A(3 ,Real *);
						Ts = sub_mat(T, i, i, i+2, i+2, Ts);
						SylvCompleteP(As, Bs, Cs, X, &ss);
						H->me[0][0] = -X->me[0][0];
						H->me[1][0] = -X->me[1][0];
						H->me[2][0] = 1.0*ss;
						H->n = 1;
						H->m = 3;
						QRfactor(H, qrdiag);
						Q->n = 3;
						Q->m = 3;
						D->n = 3;
						D->m = 3;
						D2->n = 3;
						D2->m = 3;
						makeQ(H, qrdiag, Q);
						m_transp(Q, Qt);
						m_copy(Ts, D);
						m_mlt(Qt, D, D2);
						m_mlt(D2, Q, D);
						free(As->me);
						free(Bs->me);
						free(Cs->me);
						free(Ts->me);

						if(D->me[1][0] > m_norm_inf(Ts)*em || D->me[2][0] > m_norm_inf(Ts)*em)
						{
							return -1;
						}
						indexed_mult(T, Qt, i, T->n, i, 3, 0);
						indexed_mult(T, Q, 0, i+3, i, 3, 1);
						T->me[i+1][i] = 0.0;
						T->me[i+2][i] = 0.0;
						T->me[i][i] = t;

						indexed_mult(U, Q, 0, U->m, i, 3, 1);

						Ts->me = NEW_A(2 ,Real *);
						Ts = sub_mat(T, i+1, i+1, i+2, i+2, Ts);
						rot(Ts, &c, &s);
						free(Ts->me);
						G->me[0][0] = c;
						G->me[0][1] = s;
						G->me[1][0] = -s;
						G->me[1][1] = c;
						indexed_mult(T, G, 0, T->n, i+1, 2, 0);
						G->me[0][0] = c;
						G->me[0][1] = -s;
						G->me[1][0] = s;
						G->me[1][1] = c;
						indexed_mult(T, G, 0, T->m, i+1, 2, 1);
						indexed_mult(U, G, 0, U->m, i+1, 2, 1);

					}
					i = i+1;
				}
				else if(decision == 2)
				{
					if(!is_in(T->me[i][i], stable_diag) && is_in(T->me[i+2][i+2], stable_diag))
					{
						As->me = NEW_A(2 ,Real *);
						As = sub_mat(T, i, i, i+1, i+1, As);
						Bs->me = NEW_A(2 ,Real *);
						Bs = sub_mat(T, i+2, i+2, i+3, i+3, Bs);
						Cs->me = NEW_A(2 ,Real *);
						Cs = sub_mat(T, i, i+2, i+1, i+3, Cs);
						Ts->me = NEW_A(4 ,Real *);
						Ts = sub_mat(T, i, i, i+3, i+3, Ts);
						SylvCompleteP(As, Bs, Cs, X, &ss);
						H->me[0][0] = -X->me[0][0];
						H->me[0][1] = -X->me[0][1];
						H->me[1][0] = -X->me[1][0];
						H->me[1][1] = -X->me[1][1];
						H->me[2][0] = 1.0*ss;
						H->me[2][1] = 0.0;
						H->me[3][0] = 0.0;
						H->me[3][1] = 1.0*ss;
						H->n = 2;
						H->m = 4;
						/*printf("\n\nHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH\n");
						m_output(H);*/
						QRfactor(H, qrdiag);
						Q->n = 4;
						Q->m = 4;
						D->n = 4;
						D->m = 4;
						D2->n = 4;
						D2->m = 4;
						makeQ(H, qrdiag, Q);
						m_transp(Q, Qt);
						m_copy(Ts, D);
						m_mlt(Qt, D, D2);
						m_mlt(D2, Q, D);
						free(As->me);
						free(Bs->me);
						free(Cs->me);
						free(Ts->me);


						if(D->me[2][0] > m_norm_inf(Ts)*em || D->me[2][1] > m_norm_inf(Ts)*em ||
							       	D->me[3][0] > m_norm_inf(Ts)*em || D->me[3][1] > m_norm_inf(Ts)*em)
						{
							return -1;
						}
						
						indexed_mult(T, Qt, i, T->n, i, 4, 0);
						indexed_mult(T, Q, 0, i+4, i, 4, 1);
						T->me[i+2][i] = 0.0;
						T->me[i+3][i] = 0.0;
						T->me[i+2][i+1] = 0.0;
						T->me[i+3][i+1] = 0.0;

						indexed_mult(U, Q, 0, U->m, i, 4, 1);
						Ts->me = NEW_A(2 ,Real *);
						Ts = sub_mat(T, i, i, i+1, i+1, Ts);
						rot(Ts, &c, &s);
						G->me[0][0] = c;
						G->me[0][1] = s;
						G->me[1][0] = -s;
						G->me[1][1] = c;
						indexed_mult(T, G, 0, T->n, i, 2, 0);
						G->me[0][0] = c;
						G->me[0][1] = -s;
						G->me[1][0] = s;
						G->me[1][1] = c;
						indexed_mult(T, G, 0, T->m, i, 2, 1);
						indexed_mult(U, G, 0, U->m, i, 2, 1);

						Ts = sub_mat(T, i+2, i+2, i+3, i+3, Ts);
						rot(Ts, &c, &s);
						G->me[0][0] = c;
						G->me[0][1] = s;
						G->me[1][0] = -s;
						G->me[1][1] = c;
						indexed_mult(T, G, 0, T->n, i+2, 2, 0);
						G->me[0][0] = c;
						G->me[0][1] = -s;
						G->me[1][0] = s;
						G->me[1][1] = c;
						indexed_mult(T, G, 0, T->m, i+2, 2, 1);
						indexed_mult(U, G, 0, U->m, i+2, 2, 1);
						free(Ts->me);
					}
					i = i+2;
				}
			}
		}

		for(i = 0; i < subspace_size; i++)
		{
			current_diag->ve[i] = T->me[i][i];
		}
	}

	m_free(G);
	m_free(X);
	m_free(Q);
	m_free(Qt);
	m_free(H);
	m_free(D);
	m_free(D2);
	free(As);
	free(Bs);
	free(Cs);
	free(Ts);
	v_free(qrdiag);

	return 0;
}

int schur_are(MAT *A, MAT *B, MAT *Q, MAT *R, MAT *X, double *ill)
{
	int i, j;
	int n = A->n;
	int r;
	MAT *Ntmp = m_get(n, n);
	MAT *N = m_get(n, n);
	MAT *Bt = m_get(B->n, B->m);
	MAT *invR = m_get(R->n, R->m);
	MAT *U = m_get(2*n, 2*n);
	MAT *H = m_get(2*n, 2*n);
	MAT *H2 = m_get(2*n, 2*n);
	MAT *U1 = m_get(n, n);
	MAT *U2 = m_get(n, n);
	VEC *diag = v_get(n);
	VEC *U2v = v_get(n);
	
	invR = m_inverse(R, invR);
	Ntmp = m_mlt(B, invR, Ntmp);
	Bt = m_transp(B, Bt);
	m_mlt(Ntmp, Bt, N);

	/*	Constructing the Hamiltonian*/
	for(i = 0; i < n; i++)
	{
		for(j = 0; j < n; j++)
		{
			H->me[i][j] = A->me[i][j];
			H->me[i+n][j+n] = -A->me[j][i];
			H->me[i][j+n] = -N->me[i][j];
			H->me[i+n][j] = -Q->me[i][j];
		}
	}

	/*printf("\n\n########################################################\n");
	m_output(A);
	m_output(B);
	m_output(Q);
	m_output(N);
	m_output(H);*/

	schur(H, U);
	r = ordschur(H, U);
	if(r == -1)
	{
		printf("   ILL    \n");
	}

	/*m_output(H);
	m_output(U);*/
	/* extracting U11 and U21*/
	for(i = 0; i < n; i++)
	{
		for(j = 0; j < n; j++)
		{
			U1->me[j][i] = U->me[i][j];
			U2->me[j][i] = U->me[i+n][j];
		}
	}
	/*m_inverse(U1, U1);	
	m_mlt(U1, U2, X);*/
	
	QRfactor(U1, diag);
	for(i = 0; i < n; i++)
	{
		for(j = 0; j < n; j++)
		{
			U2v->ve[j] = U2->me[j][i];
		}
		QRsolve(U1, diag, U2v, U2v);
		for(j = 0; j < n; j++)
		{
			X->me[j][i] = U2v->ve[j];
		}
	}

	m_free(Ntmp);
	m_free(N);
	m_free(Bt);
	m_free(invR);
	m_free(U);
	m_free(H);
	m_free(H2);
	m_free(U1);
	m_free(U2);
	v_free(diag);
	v_free(U2v);

	*ill = (double)r;
}


