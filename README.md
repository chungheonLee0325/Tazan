# Tazan Project - 언리얼 C++ 소울라이크 게임 프로토타입

![Project Banner](khazan.png) ## 🎮 프로젝트 개요

'퍼스트 버서커: 카잔' 스타일의 전투 메커니즘을 심도 있게 학습하고 구현하는 것을 목표로 제작된 3인칭 소울라이크 액션 게임 프로토타입입니다. 언리얼 엔진 5 환경에서 C++를 적극적으로 활용하여 객체 지향 설계 원칙에 따라 핵심 시스템을 구축했습니다. 데이터 테이블을 이용한 데이터 주도 설계를 통해 게임 밸런스 및 콘텐츠 확장의 용이성을 확보했으며, 컴포넌트 기반 설계를 통해 캐릭터 기능을 모듈화하여 유지보수성을 높였습니다.

* **장르:** 3인칭 소울라이크 액션
* **개발 기간:** (2025년 2월 ~ 2025년 2월)
* **개발 인원:** (프로그래밍 3)
* **참고 게임:** 퍼스트 버서커: 카잔, 다크 소울 시리즈, 엘든 링

## ✨ 주요 기능 및 특징

* **정교한 소울라이크 전투 시스템:**
    * 스태미나 기반 공방 및 자원 관리
    * 강인도(Poise) 시스템 및 다양한 경직(Stagger) 상태 구현
    * 타이밍 기반 가드, 패리(Perfect Guard), 회피(Dodge, Perfect Dodge) 메카닉
    * 락온(Lock-On) 및 타겟 전환 기능
    * 데이터 테이블 기반 스킬 시스템 (콤보, 차지 공격 포함)
    * 힛스탑, 넉백 등 타격감 향상 요소 적용
* **AI 시스템:**
    * 몬스터별 커스텀 상태 머신(FSM) 구현
    * 언리얼 Perception System을 이용한 플레이어 감지
    * 스킬 룰렛을 통한 상황별 동적 스킬 선택 로직 (가중치 기반)
    * 보스 몬스터(예투가)의 특수 패턴 및 페이즈 구현 (돌 던지기, 차지 공격, 콤보 연계)
* **캐릭터 시스템:**
    * `AAreaObject` 기반의 공통 캐릭터 클래스 구조
    * 컴포넌트 기반 모듈식 속성 관리 (Health, Stamina, Poise, Condition)
    * 플레이어 상태(`EPlayerState`)에 따른 행동 제약 시스템
* **데이터 주도 설계:**
    * 데이터 테이블(`UDataTable`)을 활용한 캐릭터, 스킬, AI 행동 패턴 관리
* **기타:**
    * UMG 기반 UI 시스템 (상태 바, 플로팅 데미지, 락온 마커)
    * 체크포인트 기반 리스폰 시스템
    * 커스텀 유틸리티 컴포넌트 (이동, 회전, 잔상 효과)

## 🛠️ 사용 기술

* **Engine:** Unreal Engine 5.x
* **Language:** C++ (Core Gameplay Logic, Systems)
* **Blueprint:** UI Logic, Animation Blueprint State Machine, Simple Event Handling
* **Key Systems:** Enhanced Input, Animation Montage & Notify, UMG, Data Tables, Custom FSM, Custom Skill System

## 📂 프로젝트 구조 (간략)