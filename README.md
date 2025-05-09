<h2>S P _ a n d _ O S   </h2>
<br/>
<h2> 1. Web으로 시작하는 파일 -> 시스템 프로그래밍(system programming) 과제: LINUX환경에서 WEB SERVER 구현 </h2>
   - 과제 제안서 모음 directory : sp_proposal 
   <br/>
 <h3> -1차 과제 : linux의 기존 ls를 직접 구현하여 ./spls_final command를 치면 동일한 작동을 하게끔 한다. </h3>
 <hr/>
   해당 디렉토리 내의 파일들을 보여주는 것은 물론, -a/-l/-la/-h/-r/-S option을 구현한다. 
  <br/>
   command 뒤에 '*', '?'. '[seq]' 에 알맞게 ls 결과가 정렬되어 나오도록 구현한다.    
  <br/>
   [결과 예시 이미지]
   <img src="images/스크린샷 2025-04-29 143452.png" alt="결과 이미지" width="500"/>
  <br/>

   <h3>-2차 과제 :  spls_final command의 결과를 terminal이 아닌 html 파일에 display 해주도록 구현한다.</h3> <hr/>

   클라이언트와 소켓이 연결된 이후의 작업을 fork()를 통해 자식 프로세스에게 수행하게 하여 동시에 다수 클라이언트가 접속할 수 있도록 지원하는 web server를 구현한다.<br/> 10초에 한번씩 연결되었던 클라이언트의 정보를 접속 정보 및 끊긴 정보를 terminal에 출력해준다. <br/> 보안성 및 신뢰성을 위해 미리 허용된 ip 주소 범위 가 아닐 시 request 오류를 출력해준다.<br/>
[결과 예시 이미지]<br/>
   <img src="images/스크린샷 2025-04-29 151935.png" alt="결과 이미지" width="500"/>
<br/>
[결과 예시 이미지]<br/>
   <img src="images/스크린샷 2025-04-29 152109.png" alt="결과 이미지" width="500"/>
<br/>

<h3>-3차 과제 : Advanced Web Server with processor pool</h3> <hr/>

지난 차수는 client를 accept한 뒤에 fork를 한 것과 달리 미리 5개의 child process를 fork한 뒤, 그 process에서 요청이 온 client와 연결시켜준다. 10초마다 부모 프로세서에서 history 목차를 출력하고, 해당 목차에 맞게 구조체 배열을 출력해주는 일은 자식 프로세스가 해준다. 프로세스는 최대 10개의 history를 가지며, 자식 프로세스에 종료 시그널을 보내 적절히 프로세스들을 종료시킨다.<br/>

[결과 예시 이미지]
<img src="images/스크린샷 2025-04-29 153118.png" alt="결과 이미지" width="500"/>
     
<br/><br/>
<h2> 2. OS로 시작하는 파일 -> 운영체제 수업 과제 : LINUX 커널 분석 및 응용 과제 </h2> 
- 과제 제안서 모음 directory: os_proposal<br/>   
<h3>-1차 과제 : kernel 5.4.282버전을 사용하기 위해 환경 설정을 한 뒤, compile을 해준다. ctags, cscope 실습을 한다. </h3> <hr/>

<h3>-2차 과제 : Hijacking, Wrapping, module creation </h3> <hr/>
system call table 336번에 Asmlinkage int os_ftrace(pid_t pid)를 생성한다. 이 336번을 hijack하여 my_ftrace 함수로 대체하는 module을 구현한다. module안에서 특정 pid에 대해 기존 openat, read, write, lseek, close 코드(cscope -R로 찾기)를 내가 만든 함수로 hijacking 및 wrapping하는 코드를 구현한다. 여기서 포인트는 module의 init함수에서 모듈을 적재할 때 기존에 구현되어 있던 함수의 주소를 따로 저장한 뒤, 나만의 함수로 hooking을 한 뒤, 다시 exit함수에서 부여했던 권한을 다시 회수한 뒤 함수의 주소를 반환해주면서 커널에 영향을 주지 않도록 안전하게 짜는 것이 중요하다.
<br/>
[결과 예시 이미지]
<img src="images/스크린샷 2025-04-29 162828.png" alt="결과 이미지" width="500"/>
<h3>-3차 과제: Advanced module and 4 CPU scheduling algorithm coding </h3> <hr/>

2차 과제에서 만든 336번 system call table에 tast_struct 구조체를 통해 target pid에 대한 정보를 출력하는 trace하는 code구현 및 temp.txt 파일에 나열된 숫자열 중 앞의 두개씩 두 숫자의 합 연산을 하는 코드 구현, CPU scheduling algorithm(FCFS, SJF, RR, SRTF)를 구현한 뒤 간트 차트로 결과를 출력하는 코드를 구현한다.<br/>
[결과 예시 이미지]
<img src="images/스크린샷 2025-04-29 163511.png" alt="결과 이미지" width="500"/>
<img src="images/스크린샷 2025-04-29 163447.png" alt="결과 이미지" width="500"/>
<img src="images/스크린샷 2025-04-29 163409.png" alt="결과 이미지" width="500"/>
<h3>-4차 과제 : 가상 메모리 정보 출력, 분석 및 4 memory swapping policy coding </h3> <hr/>

336번 system call을 hooking하여 task_struct의 vm_area_struct를 가리키는 mm_struct를 통해 가상 메모리에 접근하여(vm_next, vm_prev로 끝까지 스캔하면서) vm_start, vm_end를 통해 가상 메모리의 페이지 시작과 끝 부분을 출력한다. strt_code와 end_code/ start_data, end_data, start_brk 등을 통해 코드영역, data영역, heap 영역의 주소를 출력한다. d_path함수를 활용하여 mmap의 vm_file의 path를 찾아내어 출력한다. swapping algorithm의 경우, optimal, FIFO, LRU, Clock algorithm을 구현 후, 각 알고리즘마다의 page fault rate를 구해 비교한다.<br/>
[결과 예시 이미지]
<img src="images/스크린샷 2025-04-29 164340.png" alt="결과 이미지" width="500"/>
<h3>-5차 과제 : FAT filesystem 구현</h3> <hr/>

FAT filesystem을 간단하게 구현하는 코드로, create,write,read,list,delete를 구현하
고, load 및 save를 실행한다. FAT table은 file data를 저장하고, 각 블록은 다음 블
록을 가리킨다. 마지막 블록은 –1을 가진다. block size는 32byte로, data가 block 
size보다 크면, 새 block을 할당하여 write해준다. 읽고, 쓸 때는 다음 블록으로 이동
하면서 data에 접근한다.
   <br/><br/>
 
