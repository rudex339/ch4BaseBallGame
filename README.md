# ch4BaseBallGame
간단하게 30초의 시간이 주어지고 자신의 턴마다 숫자를 불러서 비밀 숫자를 맞춰야하는
숫자 야구 게임을 만들었습니다

강의를 따라가면서 대략적으로 정의를 해보았는데
게임모드는 서버에 하나밖에 없고 게임모드와 상호작용을 할려면  UPROPERTY(Replicated), 
UFUNCTION(Client or Server, Reliable)을 사용해서 
서버에서 접근 가능한 변수 서버에 접근 or 서버에서 접근가능한 함수를 만들어야한다.
replicated 변수는 GetLifetimeReplicatedProps에서 DOREPLIFETIME(ThisClass, 변수);이거를 해줘야만 한다
게임모드에서 설정해야하는건 아직은 없는듯