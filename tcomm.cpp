#include<mpi.h>
#include <sys/time.h>
#include <unistd.h>
using namespace std ;
//typedef duration<long long> seconds;
typedef struct timeval timeval ;

void LinReg(double *duration ) //這個函式是用來算出拿條線的截距和斜率
{
    double ymean = 0 ,xmean ;
    for (int x = 0 ; x < 5 ; x++ )
        ymean += duration[x] ;
    ymean /= 5 ; xmean = 3 ;
    double x1 = 0 , x2 = 0 ;
    for(int x = 1 ; x <= 5 ; x++)
    {
        x1 += ( (double)x * duration[x-1]) ;
        x2 += ( x * x )  ; 
    }
    x1 = (x1 - 5*xmean*ymean)/ ((double)x2 - 5*xmean*xmean) ;
    duration[5] = x1 ;
    duration[6] = ymean - x1*xmean ;
}

int main(int argc , char *argv[] )
{

    int npes , myrank ; //npes 是總共幾個cpu myrank 是用來控制我要用第幾個cpu 來平行運算
    double duration[7] ;  // 儲存時間用的

    // 下面這三行是用來MPI需要的初始化
    MPI_Init( &argc , &argv ) ;
    MPI_Comm_size( MPI_COMM_WORLD , &npes ) ;
    MPI_Comm_rank( MPI_COMM_WORLD, &myrank ) ;

    for (int x = 1 ; x <=5 ; x++ ) //總共跑五次 分別是100000 - 500000
    {
        sleep(1) ; //休息一秒
        MPI_Status status ;
        timeval START , END ;  // time variable 
        gettimeofday(&START,NULL); // start 
        for(int y = 0 ; y < 1000 ; y++)// 跑1000次
        {
            int a[x*100000] , b[x*100000] ; // 我要從 a 陣列分別傳 100000 - 5000000 個東西給 b 
            
            if(myrank == 0) // 當myrank 是 0 時 ， cpu 0 運作以下東西 
            {
                //a[0] = 100 ; a[1] = 200 ;
                MPI_Send(a, x*100000 ,MPI_INT,1,1,MPI_COMM_WORLD); // 從a送 x * 100000 的陣列給 b 
                MPI_Send(&(START.tv_usec),1,MPI_DOUBLE,1,2,MPI_COMM_WORLD); 
                //printf("my rank : %d , START : %lf \n" ,myrank , (double)START.tv_usec) ;
            }
            else 
            {
                //a[0] = 300 ; a[1] =  400 ;
                MPI_Recv( b, x*100000 ,MPI_INT,0,1,MPI_COMM_WORLD,&status ) ; // 從b 接收 x * 100000 的陣列從 a 
                MPI_Recv(&(START.tv_usec),1,MPI_DOUBLE,0,2,MPI_COMM_WORLD , &status );
                //printf("my rank : %d , START : %lf \n "  , myrank , (double)START.tv_usec) ;
            }

        }
        gettimeofday(&END,NULL); //end 
        if (myrank == 1 )
        {
            duration[x-1] =  ((END.tv_sec - START.tv_sec)  + (double)(END.tv_usec - START.tv_usec)/1000000.0 )/1000.0 ;  //計算每個時間並儲存到duration
            printf( "%lf msec \n",duration[x-1]) ;
        }
        
    }

    MPI_Finalize() ;

    if(myrank == 1 )
    {
        LinReg(duration ) ; // 算出 duration 的 linear regression 
        printf("y = %lfx + %lf \n" , duration[5] , duration[6] ) ; //  我的duration[0-4] 這五個是用來存時間，5-6是用來存截距和斜率 y = ax + b
    }
}