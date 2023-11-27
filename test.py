# """ 1 task """
# # print(len([[]]))

# """ task 2 """
# M = ["Ann", "Sam", "Kate", "Ali"]
# # print(sorted(i[-1] for i in M))

# """ task3  """
# # var = (i for i in range(1))
# # next(var)
# # print(next(var))

# """ task 4 """
# # print( list(map(str.upper, M)) )

# """ task 5 """
# # l1 = [1,2,3]
# # l2 = 'abcd'
# # print(list(zip(l1,l2)))

# """ task 6 """
# def b_f(text):
# #     text = "".join([i for i in text.lower() if i.isalpha() or i == " "])
# #     text1 = text.split(" ")
# #     d = {" ".join(text1[i:i+2]): text.count(" ".join(text1[i:i+2])) for i in range(len(text1) - 1)}
# #     d1 = {key: round(d[key] / sum(d.values()), 1)  for key in d}
# #     return d1

# print(b_f("Beth is a student. Asd is a student. 32 is a number"))


def replace_elements(n, arr):
    stack = []
    result = [0] * n

    for i in range(n - 1, -1, -1):
        while stack and arr[i] >= arr[stack[-1]]:
            stack.pop()

        if stack:
            result[i] = arr[stack[-1]]

        stack.append(i)

    return result

# Зчитуємо вхідні дані
n = int(input())
arr = list(map(int, input().split()))

# Отримуємо результат і виводимо його
result = replace_elements(n, arr)
print(*result)
