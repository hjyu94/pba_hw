## Discussion

contact 이 있을 때 두 물체의 충돌의 normal 방향에 대해 realtive velocity 가 0인 경우(HW6 코드에서는 numerical tolerance 를 고려해 상대속도의 절대값이 epsilon 보다 작은지로 체크하였습니다), 두 물체는 멀어지지도 않고 더 가까워지지도 않는 resting 상태에 놓이게 된다.이 때 각각의 contact point 에서 contact surface 에 수직으로 작용하는 contact force 를 갖게 된다. resting 상태에서 contact force 는 두 물체가 더 이상 가까워져 뚫고 들어가는 현상을 막아준다.

만약 공이 바닥으로 떨어지는 시뮬레이션에서 공의 y 방향 속도가 epsilon 보다 작아지는 resting 현상을 구현하는 경우에, force level 이 아니라 velocity level 에서 구현하려고 할 수도 있다. 즉 비탄성 충돌을 하였다고 보고 충돌 후의 속도가 0이 되도록 구현하려고 할 수도 있다. 하지만 충돌 직후에는 contatct surface 의 normal 방향의 velocity 가 적절히 계산되겠지만, 작용하고 있는 힘이 사라지지 않았기 때문에 공이 점차 바닥을 뚫고 들어가는 모습을 볼 수 있다.

전체적인 시뮬레이션이 어떻게 진행되는지를 설명해보자면, 중력을 받아서 떨어지던 공이 바닥과 충돌하는 경우, 반발계수에 따라서 충돌 후 속력이 점점 작아지게 되고, 속력이 0에 가까워져 resting 상태에 놓이는 경우 멈춰있어야 하겠지만 바닥을 뚫고 들어가는 등속운동을 하다가 더 이상 contact 하지 않는 경우 가속도 운동을 하며 떨어지는 운동을 하게 된다.