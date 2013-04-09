m = [[1, 2, 3, 4],
     [5, 6, 7, 8],
     [9,10,11,12],
    [13,14,15,16]]

n = 4

def left():
  # forward last row
  forward = 0
  for j in range(n):
    row_val = m[n-1][j]
    diag = [row_val]
    for d in range(1, forward+1):
      diag.append(m[n-1-d][j-d])
    yield diag
    forward += 1

  # ascend last column
  ascend = n - 2
  for i in range(n-2, -1, -1):
    col_val = m[i][n-1]
    diag = [col_val]
    for d in range(1, ascend+1):
      diag.append(m[i-d][n-1-d])
    yield diag
    ascend -= 1

def right():
  # descend first column
  descend = 0
  for i in range(n):
    col_val = m[i][0]
    diag = [col_val]
    for d in range(1, descend+1):
      diag.append(m[i-d][0+d])
    yield diag
    descend += 1

  # forward last row
  forward = n - 2
  for j in range(1, n):
    row_val = m[n-1][j]
    diag = [row_val]
    for d in range(1, forward+1):
      diag.append(m[n-1-d][j+d])
    yield diag
    forward -= 1

print "\-diagonals:"
for d in left():
  print "\t", d

print "/-diagonals:"
for d in right():
  print"\t", d
