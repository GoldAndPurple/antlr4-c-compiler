## 1
```
S
T - S
T - T + S
T - T + T
F - T + F
F - F * T + F
F - F * F + F
a - b * a + b
```

```
S
'a' S B C
'aa' S B C B C
'aa ab' C B C B C
'aaa b' B B C C C
'aaa b' B B C C C
'aaa bb' B C C C
'aaa bbb' C C C
'aaa bbb c' C C
'aaa bbb cc' C
'aaa bbb ccc'
aaabbbccc
```
## 2
```
S : A B C
A : 'a' | 'a' A
B : 'b' | 'b' B
C : 'c' | 'c' C
```

```
S : Z T | Z | T | ''
Z : '0' | '0' Z
T : '10' | '10' T
```

```
S : F T
T : T F | F
F : A A
A : '0' | '1'
```

## 3
a) Контекстно-зависимая грамматика, т.к. возможна замена нетерминала А на несколько различных цепочек.

b) Pегулярная леволинейная грамматика, т.к. имеются только два вида правил: A -> A'a' и A -> 'ba', допустимые для регулярной грамматики.

## 4
```
S : S S | A S B | B S A | ''
A : 'a'
B : 'b'
```

## 5
```
S : W D
W : N | W N
D : '.' N | D N
N : '0' | '1'
```

## 6

"ε" - представление пустого символа, подходящего для "?" токена
```\b[A-Za-z_][A-Za-z0-9_]*```

https://regexr.com/6hhhq

![Graph 1](https://g.gravizo.com/svg?digraph%20G%20{%20S%20-%3E%201%20[label=%22[A-Za-z_]%22];%201%20-%3E%201%20[label=%22[A-Za-z0-9_]%22];%201%20-%3E%20end%20[label=%22%CE%B5%22];%20}
)

([0-9]+\.?[0-9]*|\.[0-9]+)([Ee][+-]?[0-9]+[FfLl]?)

https://regexr.com/6hhjv

![Graph 2](https://g.gravizo.com/svg?%20%20digraph%20G%20{S%20-%3E%201%20[label=%22[0-9]%22];1%20-%3E%201%20[label=%22[0-9]%22];S%20-%3E%202%20[label=%22\.%22];1%20-%3E%202%20[label=%22\.%22];2%20-%3E%202%20[label=%22[0-9]%22];1%20-%3E%203%20[label=%22[Ee]%22];2%20-%3E%203%20[label=%22[Ee]%22];3%20-%3E%204%20[label=%22[+-%CE%B5]%22];4%20-%3E%204%20[label=%22[0-9]%22];4%20-%3E%20end%20[label=%22[FfLl%CE%B5]%22];}
)
