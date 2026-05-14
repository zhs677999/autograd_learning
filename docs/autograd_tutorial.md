# TinyTensor 自动求导入门教程

这份文档只讲框架和思路，不要求你一次写完完整实现。建议你先跑通标量自动求导，再扩展矩阵、优化器和神经网络。

## 1. 先想清楚：自动求导到底在做什么

自动求导不是“直接求公式的导数”，而是把一次计算过程记录成一张计算图。

例如：

```cpp
Tensor x(2.0);
Tensor w(3.0);
Tensor b(1.0);
Tensor target(10.0);

Tensor wx = w * x;
Tensor y = wx + b;
Tensor diff = y - target;
Tensor loss = diff * diff;
loss.backward();
```

这段代码对应的计算图是：

```text
w ----\
      (*) ---- wx ----\
x ----/               (+) ---- y ----\
b --------------------/              (-) ---- diff ---- (*) ---- loss
target ------------------------------/                  /
diff ---------------------------------------------------/
```

前向传播负责算值：

```text
wx = 3 * 2 = 6
y = 6 + 1 = 7
diff = 7 - 10 = -3
loss = (-3) * (-3) = 9
```

反向传播负责从 `loss` 往前传梯度：

```text
d(loss)/d(w) = -12
d(loss)/d(b) = -6
```

## 2. 第一版不要单独建 Node 和 Graph

对新手来说，第一版最推荐：

```text
Tensor 本身就是 Node
```

也就是说，每个 `Tensor` 不只是保存数据，还保存：

```text
data          当前值
grad          当前梯度
parents       当前 Tensor 从哪些 Tensor 算出来
backwardFunc  当前 Tensor 怎么把梯度传回 parents
requiresGrad  是否需要梯度
```

暂时不需要单独写：

```cpp
class Node {};
class Graph {};
```

等你把最小版跑通后，再考虑这些抽象。

## 3. 推荐文件职责

### LinkedList.h

只做容器，不写自动求导逻辑。

它可以提供这些能力：

```cpp
template <typename T>
class LinkedList {
public:
    void add(T value);
    T get(int index) const;
    int size() const;
    void clear();
};
```

注意：如果你用它保存 `Tensor*`，它只是保存指针，不一定拥有这些 Tensor 的生命期。

### Tensor.h

`Tensor` 是核心类。

推荐接口长这样：

```cpp
class Tensor {
private:
    double* data;
    double* grad;
    int rows;
    int cols;

    LinkedList<Tensor*> parents;
    BackwardFunc backwardFunc;
    bool requiresGrad;

public:
    Tensor(double value, bool reqGrad = true);

    double value() const;
    double gradValue() const;

    void addParent(Tensor* p);
    Tensor* getParent(int index);
    int parentCount() const;

    void setGrad(int index, double value);
    double getGrad(int index) const;
    void addGrad(int index, double value);
    void zeroGrad();

    void setBackwardFunc(BackwardFunc f);
    void backward();
    void _backward();
};
```

第一版你可以只支持 `1 x 1` 标量。矩阵以后再说。

### Ops.h / Ops.cpp

运算层负责创建新 Tensor，并记录计算图。

每个运算都按这个模板想：

```text
1. 检查输入形状
2. 计算输出值
3. 创建输出 Tensor
4. 把输入 Tensor 加入输出 Tensor 的 parents
5. 给输出 Tensor 绑定 backwardFunc
6. return 输出 Tensor
```

框架示意：

```cpp
Tensor operator+(Tensor& a, Tensor& b) {
    Tensor out;

    // TODO 1: out.data = a.data + b.data
    // TODO 2: out.addParent(&a)
    // TODO 3: out.addParent(&b)
    // TODO 4: out.setBackwardFunc(addBackward)

    return out;
}
```

反向函数示意：

```cpp
void addBackward(Tensor* self) {
    Tensor* a = self->getParent(0);
    Tensor* b = self->getParent(1);

    // z = a + b
    // dz/da = 1
    // dz/db = 1
    // TODO:
    // a->addGrad(0, self->getGrad(0));
    // b->addGrad(0, self->getGrad(0));
}
```

## 4. 四种基础运算的梯度公式

### 加法

```text
z = a + b
dz/da = 1
dz/db = 1
```

反向传播：

```text
a.grad += z.grad
b.grad += z.grad
```

### 减法

```text
z = a - b
dz/da = 1
dz/db = -1
```

反向传播：

```text
a.grad += z.grad
b.grad -= z.grad
```

### 乘法

```text
z = a * b
dz/da = b
dz/db = a
```

反向传播：

```text
a.grad += b.data * z.grad
b.grad += a.data * z.grad
```

### 除法

```text
z = a / b
dz/da = 1 / b
dz/db = -a / (b * b)
```

反向传播：

```text
a.grad += z.grad / b.data
b.grad += -a.data * z.grad / (b.data * b.data)
```

## 5. backward() 应该怎么做

最小版思路：

```cpp
void Tensor::backward() {
    // 1. loss 的梯度设为 1
    // 2. 从 loss 出发，找到所有父节点
    // 3. 按从后往前的顺序调用 _backward()
}
```

为什么 `loss.grad = 1`？

因为：

```text
d(loss)/d(loss) = 1
```

如果计算图是：

```text
a -> b -> c -> loss
```

那反向传播顺序应该是：

```text
loss
c
b
a
```

更稳的做法是先做拓扑排序：

```text
先递归访问 parents
再把当前 Tensor 放入 topo
最后倒序遍历 topo
```

## 6. 第一阶段验收目标

先不要碰 `Linear`、`SGD`、`Dataset`。第一阶段只验收这个：

```cpp
Tensor x(2.0);
Tensor w(3.0);
Tensor b(1.0);
Tensor target(10.0);

Tensor wx = w * x;
Tensor y = wx + b;
Tensor diff = y - target;
Tensor loss = diff * diff;

loss.backward();
```

正确结果：

```text
loss = 9
w.grad = -12
b.grad = -6
```

如果这个例子跑通，说明你的自动求导核心已经通了。

## 7. 推荐开发顺序

1. 修好 `LinkedList` 的 `add/get/size/clear`
2. 让 `Tensor` 能保存标量和梯度
3. 实现 `Tensor::addParent/getParent/parentCount`
4. 实现 `+`
5. 实现 `*`
6. 实现 `backward`
7. 跑通 `z = a * b`
8. 跑通 `loss = (w*x+b-target)^2`
9. 再实现 `-` 和 `/`
10. 再做矩阵和 `matmul`
11. 再做 `mseLoss`
12. 最后做 `SGD`、`Linear`、`Dataset`

## 8. 常见坑

### 坑 1：父节点不要乱 delete

如果 `loss.parents` 里保存的是 `&w`、`&x`，那 `loss` 不应该负责删除 `w`、`x`。

父节点列表通常只是“引用关系”，不是“所有权关系”。

### 坑 2：临时 Tensor 生命周期

这句看起来很自然：

```cpp
Tensor y = w * x + b;
```

但对裸指针计算图来说，中间的 `w * x` 可能是临时对象，生命周期比较危险。

新手阶段建议先写成：

```cpp
Tensor wx = w * x;
Tensor y = wx + b;
```

这样每个中间 Tensor 都有名字，方便调试。

### 坑 3：梯度要累加，不是覆盖

如果一个 Tensor 被多个地方用到，它收到的梯度要相加。

所以反向传播通常是：

```cpp
a.grad += ...
```

不是：

```cpp
a.grad = ...
```

### 坑 4：先标量，后矩阵

矩阵求导会多出 shape、转置、广播等问题。第一版先只写标量，会轻松很多。

## 9. 你当前项目的最小路线

当前项目已经有这些文件：

```text
include/Tensor.h
include/Ops.h
include/LinkedList.h
include/Optimizer.h
include/Module.h
src/Tensor.cpp
src/Ops.cpp
src/Optimizer.cpp
src/Module.cpp
src/main.cpp
```

建议你现在只关注：

```text
LinkedList.h
Tensor.h
Tensor.cpp
Ops.h
Ops.cpp
src/main.cpp 的 basic demo
```

等 `basic` 跑通，再打开其他文件。

