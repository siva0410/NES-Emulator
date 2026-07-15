	; 스택 샘플

	; INES 헤더
	.inesprg 1 ;   - 프로그램에 몇개의 뱅크를 사용하는가. 지금은 1개
	.ineschr 1 ;   - 그래픽 데이터에 몇개의 뱅크를 사용하는가. 지금은 1개.
	.inesmir 0 ;   - 수평 미러링
	.inesmap 0 ;   - 맵퍼. 0번으로 한다.

	; 제로 페이지 변수
Scroll_X = $00	; X스크롤 값
Sound_A = $01	; A사운드 (구형파1) 카운터
Sound_B = $02   ; B사운드 (구형파2) 카운터
Sound_C = $03   ; C사운드 (삼각파) 카운터
Sound_D = $04   ; D사운드 (노이즈) 카운터
BGM_Index = $05 ; C사운드 (삼각파) 인덱스

	.bank 1      ; 뱅크 1
	.org $FFFA   ; $FFFA부터 개시

	.dw mainLoop ; VBlank 인터럽트 핸들러 (1/60초마다 mainLoop가 콜된다)
	.dw Start    ; 리셋 인터럽트. 리셋 인터럽트. 기동시와 리셋으로 Start로 점프
	.dw IRQ      ; 하드웨어 인터럽트와 소프트웨어 인터럽트에 의해 발생

	.bank 0			 ; 뱅크 0
	.org $0300	 ; $0300부터 개시,스프라이트 DMA 데이터 배치
Sprite1_Y:     .db  0   ; 스프라이트 #1 Y좌표
Sprite1_T:     .db  0   ; 스프라이트 #1 넘버
Sprite1_S:     .db  0   ; 스프라이트 #1 속성
Sprite1_X:     .db  0   ; 스프라이트 #1 X좌표
Sprite2_Y:     .db  0   ; 스프라이트 #2 Y좌표
Sprite2_T:     .db  0   ; 스프라이트 #2 넘버
Sprite2_S:     .db  0   ; 스프라이트 #2 속성
Sprite2_X:     .db  0   ; 스프라이트 #2 X좌표


	.org $8000	; $8000부터 개시
Start:
	sei			; 인터럽트 불허가
	cld			; 데시멀 모드 플러그 클리어
	ldx #$ff
	txs			; 스택 포인터 초기화 

	; PPU 컨트롤 레지스터 1 초기화
	lda #%00001000	; 여기서는 VBlank 인터럽트 금지
	sta $2000

waitVSync:
	lda $2002			; VBlank가 발생하면 $2002의 7비트째가 1이 된다
	bpl waitVSync  ; bit7이 0인 동안에는 waitVSync라벨의 위치로 점프하여 루프하며 계속 기다린다


	; PPU 컨트롤 레지스터 2 초기화
	lda #%00000110		; 초기화중에는 스프라이트와 BG를 표시 OFF로 한다
	sta $2001

    ;팔렛트를 로드
	ldx #$00    ; X레지스터 클리어

	; VRAM어드레스 레지스터인 $2006에 팔렛트의 로드처의 어드레스 $3F00을 지정한다.
	lda #$3F    ; have $2006 tell
	sta $2006   ; $2007 to start
	lda #$00    ; at $3F00 (pallete).
	sta $2006

loadPal: ; 라벨은 [라벨명+:]의 형식으로 기술
	lda tilepal, x ; A에 (ourpal + x)번지의 팔렛트를 로드한다

	sta $2007 ; $2007에 팔렛트의 값을 읽어들임

	inx ; X레지스터에 값을 1 가산하고 있다

	cpx #32 ; X를 32(10진수.BG와 스프라이트의 팔렛트의 총수)와 비교하여 같은지 어떤지를 비교하고 있다	
	bne loadPal ;	위가 같지 않은 경우는 loadpal라벨의 위치로 점프한다
	; X가 32라면 팔렛트 로드 종료

	; 속성(BG팔렛트 지정 데이터)을 로드

	; $23C0의 속성테이블에 로드한다
	lda #$23
	sta $2006
	lda #$C0
	sta $2006

	ldx #$00    ; X레지스터 클리어
	lda #%00000000				; 4개 모두 팔렛트 0번
	; 0번이나 1번으로 한다
loadAttrib
	eor #%01010101				; XOR연산으로 하나 건너의 비트를 교대로 0이나 1로 한다
	sta $2007							; $2007에 속성의 값($0이나 $55)을 읽어들임
	; 64회(전 캐릭터분)루프한다
	inx
	cpx #64
	bne loadAttrib

	; 네임테이블 생성

	; $2000의 네임테이블에 생성한다
	lda #$20
	sta $2006
	lda #$00
	sta $2006

	lda #$00        ; 0번(새까망)
	ldy #$00    ; Y레지스터 초기화
loadNametable1:
	ldx Star_Tbl, y			; Star테이블의 값을 X에 읽어들임
loadNametable2:
	sta $2007				; $2007에 속성의 값을 읽어들임
	dex							; X감산
	bne loadNametable2	; 아직 0이 아니라면 루프하여 검정을 출력한다
	; 1번이나 2번의 캐릭터를 Y의 값에서 번갈아 취득
	tya							; Y→A
	and #1					; A AND 1
	adc #1					; A에 1가산하여 1이나 2로
	sta $2007				; $2007에 속성의 값을 읽어들임
	lda #$00        ; 0번(새까망)
	iny							; Y가산
	cpy #20					; 20회(스타테이블의 수)루프한다
	bne loadNametable1

	; １번째 스프라이트 좌표 초기화
	lda X_Pos_Init
	sta Sprite1_X
	lda Y_Pos_Init
	sta Sprite1_Y
	; ２번째 스프라이트 좌표 갱신 서브루틴을 콜
	jsr setSprite2
	; ２번째 스프라이트를 수평반전
	lda #%01000000
	sta Sprite2_S

	; 제로 페이지 초기화
	lda #$00
	ldx #$00
initZeroPage:
	sta <$00, x
	inx
	bne initZeroPage

	; PPU 컨트롤 레지스터 2초기화
	lda #%00011110	; 스프라이트와 BG의 표시를 ON으로 한다
	sta $2001

	; 사운드 레지스터 초기화
	lda #0
	sta $4015

	; PPU 컨트롤 레지스터 1의 인터럽트 허가 플래그를 세운다
	lda #%10001000
	sta $2000

infinityLoop:					; VBlank 인터럽트 발생을 기다릴뿐인 무한루프
	jmp infinityLoop

mainLoop:					; 메인루프
	; 스프라이트 묘화(DMA를 이용)
	lda #$3  ; 스프라이트 데이터는 $0300번지부터이므로 3을 로드한다.
	sta $4014 ; 스프라이트 DMA레지스터에 A를 스토어하여 스프라이트 데이터를 DMA전송한다.
	
	; BG스크롤
	lda $2002			; 스크롤 값 클리어
	lda <Scroll_X	; X의 스크롤 값을 로드
	sta $2005			; X방향 스크롤（Y방향은 고정)
	inc <Scroll_X	; 스크롤 값을 가산

	; 음악과 노이즈를 울린다
	jsr playBGM
	jsr playNoise

	; 패드 I/O 레지스터의 준비
	lda #$01
	sta $4016
	lda #$00
	sta $4016

	; 패드 입력 체크
	lda $4016  ; A버튼
	pha		   ; A를 PUSH
	lda $4016  ; B버튼
	pha		   ; A를 PUSH
	lda $4016  ; Select버튼을 스킵
	lda $4016  ; Start버튼을 스킵
	lda $4016  ; UP버튼
	pha		   ; A를 PUSH
	lda $4016  ; DOWN버튼
	pha		   ; A를 PUSH
	lda $4016  ; LEFT버튼
	pha		   ; A를 PUSH
	lda $4016  ; RIGHT버튼
	pha		   ; A를 PUSH

	pla		   	; A를 PULL(RIGHT버튼의 내용)
	and #1     	; AND #1
	beq pull_A1	; 0이라면 Pull_A1으로 점프
	jsr RIGHTKEYdown
pull_A1:
	pla		   	; A를 PULL(LEFT버튼의 내용)
	and #1     	; AND #1
	beq pull_A2	; 0이라면 Pull_A2으로 점프
	jsr LEFTKEYdown
pull_A2:
	pla		   	; A를 PULL(DOWN버튼의 내용)
	and #1     	; AND #1
	beq pull_A3	; 0이라면 Pull_A3으로 점프
	jsr DOWNKEYdown
pull_A3:
	pla		   	; A를 PULL(UP버튼의 내용)
	and #1     	; AND #1
	beq pull_A4	; 0이라면 Pull_A4으로 점프
	jsr UPKEYdown
pull_A4:
	pla		   	; A를 PULL(B버튼의 내용)
	and #1     	; AND #1
	beq pull_A5	; 0이라면 Pull_A5으로 점프
	jsr BKEYSound
pull_A5:
	pla		   	; A를 PULL(A버튼의 내용)
	and #1     	; AND #1
	beq setSpr	; 0이라면 setSpr으로 점프
	jsr AKEYSound


setSpr:
	; 2번째 스프라이트 좌표갱신 서브루틴을 콜
	jsr setSprite2
	
	; 사운드 대기 카운터 A~D(제로 페이지에서 연속된 영역이라는 전제)를 각각 -1 감산한다
	ldx #0
dec_Counter
	lda <Sound_A,x
	beq dec_Next
	dec <Sound_A,x 
dec_Next:	
	inx
	cpx #4			; 4회 반복한다
	bne dec_Counter
	
NMIEnd:
	rti				; 인터럽트로부터 복귀

UPKEYdown:
	dec Sprite1_Y	; Y좌표를 1감산
	rts

DOWNKEYdown:
	inc Sprite1_Y ; Y좌표를 1가산
	rts

LEFTKEYdown:
	dec Sprite1_X	; X좌표를 1감산
	rts

RIGHTKEYdown:
	inc Sprite1_X	; X좌표를 1가산
	rts

AKEYSound:
	; 사운드 대기 카운터 A가 0이 아닌 경우는 사운드를 울리지 않는다
	lda <Sound_A
	beq .soundSub
	rts
.soundSub
	lda #10			; 1/6초에 1회 울린다
	sta <Sound_A

	lda $4015		; 사운드 레지스터
	ora #%00000001	; 구형파 채널1을 유효로 한다
	sta $4015

	lda #%10111111
	sta $4000		; 구형파 채널 1 제어 레지스터 1

	lda #%10101011
	sta $4001		; 구형파 채널1 제어 레지스터 2
	lda Sprite1_X		; 재미삼아 X좌표를 넣어 본다
	sta $4002		; 구형파 채널 1 주파수 레지스터 1

	lda #%11111011
	sta $4003		; 구형파 채널 1 주파수 레지스터 2

	rts

BKEYSound:
	; 사운드 대기 카운터 B가 0이 아닌 경우에는 사운드를 울리지 않는다
	lda <Sound_B
	beq .soundSub
	rts
.soundSub
	lda #10			; 1/6초에 1회 울린다
	sta <Sound_B

	lda $4015		; 사운드 레지스터
	ora #%00000010	; 구형파 채널 2를 유효로 한다
	sta $4015

	lda #%10111111
	sta $4004		; 구형파 채널 2 제어 레지스터 1

	lda #%10000100
	sta $4005		; 구형파 채널 2 제어 레지스터 2
	lda Sprite1_Y		; 재미삼아 Y좌표를 넣어 본다
	sta $4006		; 구형파 채널 2 주파수 레지스터 1

	lda #%11111000
	sta $4007		; 구형파 채널 2 주파수 레지스터 2

	rts

playBGM:
	; 사운드 대기 카운터 C가 0이 아닌 경우는 사운드를 울리지 않는다
	lda <Sound_C
	beq .soundSub
	rts
.soundSub
	ldx BGM_Index					; BGM테이블 인덱스를 X에 읽어들임
	lda BGM_Tbl, x					; BGM테이블에서 대기 카운터를 읽어들임
	sta <Sound_C					; 대기 카운터를 설정
	inx								; BGM테이블 인덱스 전진한다

	lda #%11111111
	sta $4008		; 삼각파 채널 제어 레지스터

	lda BGM_Tbl, x					; BGM테이블로부터 음계를 읽어들임
	sta $400A		; 삼각파 채널 주파수 레지스터 1

	lda #%11111001 
	sta $400B		; 삼각파 채널 주파수 레지스터 2

	inx								; BGM테이블 인덱스 전진한다
	cpx #20							; 마지막에 도착？
	bne playBGMEnd
	ldx #0							; 인덱스를 처음으로 되돌린다
playBGMEnd:
	stx BGM_Index					; BGM테이블 인덱스를 X에 읽어들임

	rts

playNoise
	; 사운드 대기 카운터 D가 0이 아닌 경우는 사운드를 울리지 않는다
	lda <Sound_D
	beq .soundSub
	rts
.soundSub:
	lda #60			; 1초에 1회 울림
	sta <Sound_D

	lda #%11101111
	sta $400C		; 노이즈 제어 레지스터

	lda Sprite1_X	; 재미삼아 X좌표를 넣어본다
	sta $400E		; 노이즈 주파수 레지스터 1

	lda #%11111111
	sta $400F		; 노이즈 주파수 레지스터 2

	rts

setSprite2:
	; 2번째 스프라이트의 좌표갱신 서브루틴
	clc					;　adc의 전에 캐리플러그를 클리어
	lda Sprite1_X
	adc #8 		; 8도트 오른쪽으로 물린다
	sta Sprite2_X
	lda Sprite1_Y
	sta Sprite2_Y
	rts

IRQ:
	rti

	; 초기 데이터
X_Pos_Init   .db 20       ; X좌표 초기값
Y_Pos_Init   .db 40       ; Y좌표 초기값

	; 스타테이블 데이터(20개)
Star_Tbl    .db 60,45,35,60,90,65,45,20,90,10,30,40,65,25,65,35,50,35,40,35

	; BGM테이블 데이터(대기 카운터&음계가 10개)
BGM_Tbl		.db 20,$80,10,$10,30,$04,20,$40,10,$80,10,$20,20,$20,20,$30,10,$10,30,$00

tilepal: .incbin "giko2.pal" ; 팔렛트를 include한다

	.bank 2       ; 뱅크 2
	.org $0000    ; $0000부터 개시

	.incbin "giko2.bkg"  ; 배경 데이터의 바이너리 파일을 include한다
	.incbin "giko2.spr"  ; 스프라이트 데이터의 바이너리 파일을 include한다
