#include<stdio.h>
int fib(int n)
{
  int first = 0, second = 1, next=0, c;



  for ( c = 0 ; c < n ; c++ )
    {
      if ( c <= 1 )
	next = c;
      else
	{
	  next = first + second;
	  first = second;
	  second = next;
	}

    }
  return next;
}


int fib2(int n)
{
  int a = 0, b = 1, c, i;
  if( n == 0)
    return 0;
  if (n==1)
    return 1;
  for (i = 2; i <= n; i++)
    {
      c = a + b;
      a = b;
      b = c;
    }
  return b;
}

int fib_iter(int n);

int main(void)
{
  int i,j,n,n2;
  printf("\nAssembly result: ");
  for(i=0;i<20;i++)
    {
      n=fib_iter(i);
      printf("\n%d",n);
    }

  printf("\n C result: ");
  for(i=0;i<20;i++)
    {
      n=fib2(i);
      printf("\n%d",n);
    }

  return 0;
}
