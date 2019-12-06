# Program to display the Fibonacci sequence up to n-th term
nterms = 10
# first two terms
n1 = 0
n2 = 1
count = 0
# check if the number of terms is valid
if nterms <= 0:
   print('Please enter a positive integer')
else:
   print('Fibonacci sequence:')
   while count < nterms:
       print(n1)
       nth = n1 + n2
       # update values
       n1 = n2
       n2 = nth
       count = count + 1
