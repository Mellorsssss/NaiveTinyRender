### Naive Tiny Render
A render made for fun.

def line_Bresenham(p_list):
    x0, y0 = p_list[0]
    x1, y1 = p_list[1]
    result = []
    dy = y1 - y0
    dx = x1 - x0
    absDy = abs(dy)
    absDx = abs(dx)
    x = 0
    y = 0
    x_e = 0
    y_e = 0
    # 决策变量初始值
    px = 2 * absDy - absDx
    py = 2 * absDx - absDy

    # 计算必须的一些常量
    const_2dy_m_2dx = 2 * absDy - 2 * absDx
    const_2dx_m_2dy = 2 * absDx - 2 * absDy
    const_2dy = 2 * absDy
    const_2dx = 2 * absDx

    # 斜率绝对值小于等于1
    if absDy <= absDx:
        if dx >= 0:
            x = x0
            y = y0
            x_e = x1
        # 区间转化为从左到右
        else:
            x = x1
            y = y1
            x_e = x0
        result.append((x, y))
        x_b = x
        # 遍历x区间
        for x in range(x_b + 1, x_e + 1):
            if px < 0:
                px += const_2dy
            else:
                px += const_2dy_m_2dx
                # 如果当前斜率为正
                if (dx < 0 and dy < 0) or (dx > 0 and dy > 0):
                    y += 1
                else:
                    y -= 1
            result.append((x, y))
    # 斜率绝对值大于1
    else:
        if dy >= 0:
            x = x0
            y = y0
            y_e = y1
        else:
            x = x1
            y = y1
            y_e = y0
        result.append((x, y))
        y_b = y
        # 遍历y区间
        for y in range(y_b + 1, y_e + 1):
            if py < 0:
                py += const_2dx
            else:
                py += const_2dx_m_2dy
                # 如果当前斜率为正
                if (dx < 0 and dy < 0) or (dx > 0 and dy > 0):
                    x += 1
                else:
                    x -= 1
            result.append((x, y))
    return result