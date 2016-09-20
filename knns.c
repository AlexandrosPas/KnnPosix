/**************************************************
Serial_code was given to us in order to proceed to
implement parallelism.
Pthread code by Alexandros Paschos 5964.
Guided by https://computing.llnl.gov/tutorials/pthreads/
tutorial.
All commented lines contain pthread modification of
initial serial code.
Special thanks to Alexis Provos for some remarks
and assistance with POSIX thread programming.
***************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "utils.h"
//#define NUM_THREADS 4   //Define the number of threads

struct compdata
{
    int Q, N, D;
    double *data, *query, *dist;
    int id, start, end;
};

struct seldata
{
    int id, start, end, *NNidx;
    double *NNdist, *dist;
    int N, k;
};

/***************************** A L L     F U N C S ******************************************/
double euclidean_distance(double *X, double *Y, int N);
void compute_distance(knn_struct* queries, knn_struct* dataset, double* dist, int NUM_THREADS);
void *compute_distance_callThd(void *comp_data_arg);
int findMax(double* X, int k);
void kselect(double* dist, double* NNdist, int* NNidx, int N, int k);
void selection(double* dist, double* NNdist, int* NNidx, int N, int Q, int k, int NUM_THREADS);
void *selection_callThd(void *sel_data_arg);
void knns(knn_struct* queries, knn_struct* dataset, double *NNdist, int *NNidx, int k, int NUM_THREADS);


double euclidean_distance(double *X, double *Y, int N)
{

    int i = 0;
    double dst = 0;

    for(i=0; i<N; i++)
    {
        double tmp = (X[i] - Y[i]);
        dst += tmp * tmp;
    }

    return(dst);
}
/********************************************************************
*   The function                        will be pthread-implemented.*
*********************************************************************
*                 c o m p u t e    d i s t a n c e                  *
*********************************************************************/

void compute_distance(knn_struct* queries, knn_struct* dataset, double* dist, int NUM_THREADS)
{

    int Q = queries->secondary_dim;
    int N = dataset->secondary_dim;
    int D = dataset->leading_dim;
    int i;
    double *data = dataset->data;
    double *query = queries->data;
    void *status;                     //status void pointer
    int step=N/NUM_THREADS;

    pthread_t callThd[NUM_THREADS]; //Call threads for compute_distance
    pthread_attr_t attr;              //Pthread attributes
    pthread_attr_init(&attr);         //Pthread attributes initialization
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);  //Pthread set JOINABLE
    struct compdata tdata[NUM_THREADS];

    tdata[0].start=0;
    tdata[0].end=step-1;
    tdata[0].id=0;
    tdata[0].data=(double *)dataset->data;
    tdata[0].query=(double *)queries->data;
    tdata[0].dist=(double *)dist;
    tdata[0].Q=Q;
    tdata[0].N=N;
    tdata[0].D=D;
    pthread_create(&callThd[0], &attr, compute_distance_callThd, (void *)&tdata[0]);
    for(i=1; i<NUM_THREADS; i++)
    {
        tdata[i].start=tdata[i-1].end+1;
        tdata[i].end=tdata[i].start+step-1;
        if(i+1==NUM_THREADS)
        {
            tdata[i].end=N-1;
        }
        tdata[i].id=i;
        tdata[i].data=(double *)dataset->data;
        tdata[i].query=(double *)queries->data;
        tdata[i].dist=(double *)dist;
        tdata[i].Q=Q;
        tdata[i].N=N;
        tdata[i].D=D;
        pthread_create(&callThd[i], NULL, compute_distance_callThd, (void *)&tdata[i]);
    }

    pthread_attr_destroy(&attr);
    for(i=0; i<NUM_THREADS; i++)
    {
        pthread_join(callThd[i], &status); //Joining threads of compute_distance
    }
}
void *compute_distance_callThd(void *comp_data_arg)
{
    int i, j, qi, start, end, id, Q, N, D;
    double *data, *query, *dist;

    struct compdata *thread_data;
    thread_data=(struct compdata *) comp_data_arg;

    start=thread_data->start;
    end=thread_data->end;
    id=thread_data->id;
    data=thread_data->data;
    query=thread_data->query;
    dist=thread_data->dist;
    Q=thread_data->Q;
    N=thread_data->N;
    D=thread_data->D;

    for(i=start; i<=end; i++)
    {
        for(qi=0; qi<Q; qi++)
        {
            dist[qi*N + i] = euclidean_distance(&data[i*D], &query[qi*D], D);
        }
    }
    pthread_exit(&id);
}
/************************************************
****************   E   N   D   ******************
******  C o m p u t e    d i s t a n c e  *******
*************************************************/

int findMax(double* X, int k)
{

    int i=0;
    int maxidx = 0;
    double maxval = X[0];

    for(i=1; i<k; i++)
    {

        if(maxval<X[i])
        {
            maxval = X[i];
            maxidx = i;
        }
    }

    return(maxidx);

}

void kselect(double* dist, double* NNdist, int* NNidx, int N, int k)
{


    int i = 0;

    for(i=0; i<k; i++)
    {
        NNdist[i] = dist[i];
        NNidx[i] = i;
    }

    int maxidx = findMax(NNdist, k);

    for(i=k; i<N; i++)
    {

        if(NNdist[maxidx]>dist[i])
        {
            NNdist[maxidx] = dist[i];
            NNidx[maxidx] = i;
            maxidx = findMax(NNdist, k);
        }
    }

}
/********************************************************************
This function will also be pthread-implemented
*********************************************************************
*                  S  E   L   E   C   T   I   O   N                 *
*********************************************************************
*********************************************************************/

void selection(double* dist, double* NNdist, int* NNidx, int N, int Q, int k, int NUM_THREADS)
{

    int i = 0, j = 0;
    void *status;

    int step=Q/NUM_THREADS;

    pthread_t callThd[NUM_THREADS];
    pthread_attr_t attr;
    struct seldata tdata[NUM_THREADS];
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    tdata[0].start=0;
    tdata[0].end=step-1;
    tdata[0].id=0;
    tdata[0].NNdist=(double *)NNdist;
    tdata[0].NNidx=(int *)NNidx;
    tdata[0].dist=(double *)dist;
    tdata[0].N=N;
    tdata[0].k=k;
    pthread_create(&callThd[0], &attr, selection_callThd, (void *)&tdata[0]);
    for(i=1; i<NUM_THREADS; i++)
    {
        tdata[i].start=tdata[i-1].end+1;
        tdata[i].end=tdata[i].start+step-1;
        if(i+1==NUM_THREADS)
        {
            tdata[i].end=Q-1;
        }
        tdata[i].id=i;
        tdata[i].NNdist=(double *)NNdist;
        tdata[i].NNidx=(int *)NNidx;
        tdata[i].dist=(double *)dist;
        tdata[i].N=N;
        tdata[i].k=k;
        pthread_create(&callThd[i], NULL, selection_callThd, (void *)&tdata[i]);
    }

    pthread_attr_destroy(&attr);
    for(i=0; i<NUM_THREADS; i++)
    {
        pthread_join(callThd[i], &status);
    }
}
void *selection_callThd(void *sel_data_arg){
    int i, start, end, id, N, k, *NNidx;
    double *dist, *NNdist;

    struct seldata *thread_data;
    thread_data=(struct seldata *) sel_data_arg;
    start=thread_data->start;
    end=thread_data->end;
    id=thread_data->id;
    NNdist=thread_data->NNdist;
    NNidx=thread_data->NNidx;
    dist=thread_data->dist;
    N=thread_data->N;
    k=thread_data->k;
    for(i=start; i<=end; i++)
    {
        kselect(&dist[i*N], &NNdist[i*k], &NNidx[i*k], N, k);
    }
    pthread_exit(&id);
}
/*****************************************************
*****************    E   N   D   *********************
***************  S E L E C T I O N  ******************
******************************************************/
void knns(knn_struct* queries, knn_struct* dataset, double *NNdist, int *NNidx, int k, int NUM_THREADS)
{

    double *dist;
    int q = queries->secondary_dim;
    int n = dataset->secondary_dim;

    dist = (double*)malloc(n*q*sizeof(double));

    compute_distance(queries, dataset, dist, NUM_THREADS);

    selection(dist, NNdist, NNidx, n, q, k, NUM_THREADS);

    free(dist);
}





