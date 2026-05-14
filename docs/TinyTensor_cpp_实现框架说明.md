# TinyTensor C++ 实现框架说明

这份文档说明 `Tensor.cpp`、`Ops.cpp` 应该写什么，以及“有系数时梯度怎么传”。它是写代码前的施工图，不直接替代源码。

## 1. 先明确文件职责

### `Tensor.cpp`

`Tensor.cpp` 不负责具体数学公式，它负责 Tensor 自己的基础能力：

- 梯度访问：`setGrad`、`getGrad`、`addGrad`
- 梯度清零：`zeroGrad`
- 计算图连接：`addParent`、`getParent`、`parentCount`
- 反向传播入口：`backward`、`_backward`
- 打印：`print`、`printGrad`

可以理解为：

```text
Tensor.cpp 管“节点自己怎么保存信息、怎么启动反向传播”
Ops.cpp 管“每种运算的前向公式和反向公式”
```

### `Ops.cpp`

`Ops.cpp` 负责每个运算：

- `operator+`
- `operator-`
- `operator*`
- `operator/`
- 后续的 `matmul`
- 后续的 `relu`
- 后续的 `sigmoid`
- 后续的 `mseLoss`

每个运算都做 4 件事：

```text
1. 前向计算 out.data
2. 创建结果 Tensor out
3. 记录 out 的父节点 parents
4. 绑定 out 的 backwardFunc
```

## 2. `Tensor.cpp` 建议框架

`Tensor::backward()` 的第一版可以先只支持标量 loss。

逻辑是：

```cpp
void Tensor::backward() {
    // 1. loss 对自己的导数是 1
    setGrad(0, 1.0);

    // 2. 从 loss 开始反向传播
    _backward();
}
```

然后 `_backward()`：

```cpp
void Tensor::_backward() {
    // 1. 当前节点先执行自己的反向传播规则
    if (backwardFunc) {
        backwardFunc(this);
    }

    // 2. 再递归让父节点继续反向传播
    for (int i = 0; i < parentCount(); i++) {
        Tensor* parent = getParent(i);
        if (parent) {
            parent->_backward();
        }
    }
}
```

注意：这个是“学习版框架”。完整版本最好做拓扑排序，避免复杂图里重复传播顺序出问题。

## 3. `Ops.cpp` 建议框架

以乘法为例：

```cpp
Tensor operator*(Tensor& a, Tensor& b) {
    // 1. 前向计算
    Tensor out(a.value() * b.value());

    // 2. 记录父节点
    out.addParent(&a);
    out.addParent(&b);

    // 3. 绑定反向传播函数
    out.setBackwardFunc(mulBackward);

    return out;
}
```

对应的反向函数：

```cpp
void mulBackward(Tensor* self) {
    Tensor* a = self->getParent(0);
    Tensor* b = self->getParent(1);

    double upstream = self->getGrad(0);

    a->addGrad(0, b->value() * upstream);
    b->addGrad(0, a->value() * upstream);
}
```

这里的 `upstream` 就是上一层传下来的梯度，也就是：

```text
d(loss) / d(self)
```

## 4. 有系数怎么办

你担心“是不是默认每一项系数都是 1”，这个担心很对。自动求导没有默认系数为 1，系数会通过乘法自然传下来。

比如：

```text
y = 3 * x
```

可以写成：

```cpp
Tensor c(3.0, false);
Tensor y = c * x;
```

乘法规则：

```text
y = c * x
dy/dx = c
```

所以：

```text
x.grad += c.data * y.grad
```

如果 `c = 3`，那：

```text
x.grad += 3 * y.grad
```

这就处理了系数。

## 5. 多项式例子

比如：

```text
y = 3x + 2
```

可以拆成：

```cpp
Tensor x(5.0);
Tensor c3(3.0, false);
Tensor c2(2.0, false);

Tensor term = c3 * x;
Tensor y = term + c2;
```

反向传播：

```text
dy/dterm = 1
dterm/dx = 3
所以 dy/dx = 3
```

这里加法的局部导数是 1，但乘法会把系数 3 传回来。

所以不是“所有项系数都是 1”，而是：

```text
加法节点对输入的局部导数是 1
乘法节点会把另一个乘数作为系数传回去
```

## 6. 更复杂的例子

```text
loss = (3x + 2 - target)^2
```

拆成计算图：

```cpp
Tensor c3(3.0, false);
Tensor c2(2.0, false);

Tensor term = c3 * x;
Tensor pred = term + c2;
Tensor diff = pred - target;
Tensor loss = diff * diff;
```

手算：

```text
loss = diff^2
d(loss)/d(diff) = 2 * diff
diff = pred - target
d(diff)/d(pred) = 1
pred = 3x + 2
d(pred)/d(x) = 3
```

所以：

```text
d(loss)/d(x) = 2 * diff * 3
```

这个 `3` 就是通过 `c3 * x` 的乘法反向规则传回来的。

## 7. 常量 Tensor 要不要算梯度

系数通常不需要训练，所以可以设成：

```cpp
Tensor c3(3.0, false);
```

意思是：

```text
c3.requiresGrad = false
```

反向传播时可以不更新它的梯度。

但如果这个系数是模型参数，比如线性回归里的 `w`，那就应该：

```cpp
Tensor w(3.0, true);
```

因为你希望优化器更新它。

## 8. 你现在最推荐的写法

在 `main.cpp` 的 basic demo 里，先不要写：

```cpp
Tensor y = w * x + b;
```

建议先写成：

```cpp
Tensor wx = w * x;
Tensor y = wx + b;
```

因为你现在用的是裸指针记录父节点，中间 Tensor 最好有名字，生命周期更清楚。

如果要测试系数：

```cpp
Tensor x(5.0);
Tensor c3(3.0, false);
Tensor c2(2.0, false);

Tensor term = c3 * x;
Tensor y = term + c2;
y.backward();

cout << x.gradValue() << endl; // 应该接近 3
```

