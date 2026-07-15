	; 라스터 스크롤 샘플

	; INES 헤더
	.inesprg 1 ;   - 프로그램에 몇개의 뱅크를 사용하는가. 지금은 1개
	.ineschr 1 ;   - 그래픽 데이터에 몇개의 뱅크를 사용하는가. 지금은 1개.
	.inesmir 0 ;   - 수평 미러링
	.inesmap 0 ;   - 맵퍼. 0번으로 한다.

	; 제로 페이지 변수
Scroll_X1 = $00	; 상단 스크롤 값
Scroll_X2 = $01	; 중단 스크롤 값
Scroll_X3 = $02	; 하단 스크롤 값

	.bank 1      ; 뱅크 1
	.org $FFFA   ; $FFFA부터 개시

	.dw mainLoop ; VBlank 인터럽트 핸들러 (1/60초마다 mainLoop가 콜된다)
	.dw Start    ; 리셋 인터럽트. 리셋 인터럽트. 기동시와 리셋으로 Start로 점프
	.dw IRQ      ; 하드웨어 인터럽트와 소프트웨어 인터럽트에 의해 발생

	.bank 0		; 뱅크 0

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
	lda #$3F
	sta $2006
	lda #$00
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

	ldx #$00		; X레지스터 클리어
	lda #%00000000	; ４개 모두 팔렛트 0번
	; 모두 0번으로 한다
loadAttrib
	sta $2007		; $2007에 속성의 값($0)을 읽어들임
	; 64회(전 캐릭터분)루프한다
	inx
	cpx #64
	bne loadAttrib

	; 네임테이블 생성(250+230=480회를 0번,1번의 순으로 합계 960회 기입한다)

	; 네임테이블인 $2000부터 생성한다
	lda #$20
	sta $2006
	lda #$00
	sta $2006

	lda #$00        ; 0번(투명)
	ldx #$00		; X레지스터 초기화
	jmp loadNametable2
loadNametable1:
	lda #$01        ; 1번(지면)
	ldx #$00		; X레지스터 초기화
loadNametable2:
	sta $2007		; $2007에기입한다
	inx
	cpx #250		; 250회 반복한다
	bne loadNametable2
	ldx $00
loadNametable3:
	sta $2007		; $2007에 기입한다
	inx
	cpx #230		; 230회 반복한다
	bne loadNametable3
	cmp #$01
	bne loadNametable1	; 아직 절반이므로 돌아감

	; 제로페이지 초기화
	lda #$00
	ldx #$00
initZeroPage:
	sta <$00, x
	inx
	bne initZeroPage

	; PPU컨트롤 레지스터 2 초기화
	lda #%00011110	; 스프라이트와 BG의 표시를 ON으로 한다
	sta $2001

	; PPU 컨트롤 레지스터 1의 인터럽트 허가 플래그를 세운다
	lda #%10001000
	sta $2000

	; 래스터 스크롤 개시점에 0번 스프라이트 배치
	lda #$00   ; $00(스프라이트 RAM의 어드레스는 8비트 길이)를 A에 로드
	sta $2003  ; A의 스프라이트 RAM의 어드레스를 스토어

	lda #119	; 스캔라인의 정중앙(래스터 스크롤 개시점)
	sta $2004   ; Y좌표를 레지스터에 스토어한다
	lda #00
	sta $2004   ; 0을 스토어하여 0번 스프라이트를 지정한다
	sta $2004   ; 반전이나 우선순위는 조작하지 않으므로 재차 $00를 스토어한다
	lda #0
	sta $2004   ; X좌표를 레지스터에 스토어한다

infinityLoop:					; VBlank 인터럽트 발생을 기다릴뿐인 무한루프

waitZeroSpriteClear:			; 0번 스프라이트 묘화전까지 기다림
	bit $2002
	bvs waitZeroSpriteClear		; $2002의 6비트째가 0이 될때까지 기다림
waitZeroSpriteHit:				; 0번 스프라이트 묘화까지 기다림
	bit $2002
	bvc waitZeroSpriteHit		; $2002의 6비트째가 1이 될때까지 기다림

	; BG스크롤(상단)
	lda $2002		; 스크롤 값 클리어
	lda <Scroll_X1	; 상단 스크롤 값을 로드
	lsr a			; A레지스터 우 쉬프트 (절반으로 한다)
	jsr doScrollX
	inc <Scroll_X1	; 스크롤 값을 가산

	jsr waitScan
	jsr waitScan
	jsr waitScan

	; BG스크롤(중단)
	lda $2002		; 스크롤 값 클리어
	lda <Scroll_X2	; 중단 스크롤 값을 로드
	jsr doScrollX
	inc <Scroll_X2	; 스크롤 값을 가산

	jsr waitScan
	jsr waitScan
	jsr waitScan

	; BG스크롤(하단)
	lda $2002		; 스크롤 값 클리어
	lda <Scroll_X3	; 하단 스크롤 값을 로드
	lsr a			; A레지스터 우 쉬프트(절반으로 한다)
	jsr doScrollX
	inc <Scroll_X3	; 스크롤 값을 가산
	inc <Scroll_X3	; 스크롤 값을 가산
	inc <Scroll_X3	; 스크롤 값을 가산

	jmp infinityLoop

mainLoop:			; 메인루프
	; 스크롤 고정(VBlank 인터럽트 직후에 실행하므로 다음 화면묘화의 최초부터 고정하게 된다)
	lda $2002		; 스크롤 값 클리어
	lda #$00
	jsr doScrollX

	rti				; 인터럽트로부터 복귀

doScrollX			; X방향 스크롤(A레지스터에 값 세트 완료)
	sta $2005		; X방향 스크롤
	lda #$00		; Y방향 고정
	sta $2005
	rts

waitScan			; 아무것도 하지 않고 기다림
	ldx #255
.waitScanSub
	dex
	bne .waitScanSub
	rts

IRQ:
	rti

tilepal: .incbin "giko3.pal" ; 팔렛트를 include

	.bank 2       ; 뱅크2
	.org $0000    ; $0000부터 개시

	.incbin "giko3.bkg"  ; 배경 데이터의 바이너리 파일을 include한다
	.incbin "giko2.spr"  ; 스프라이트 데이터의 바이너리 파일을 include한다
