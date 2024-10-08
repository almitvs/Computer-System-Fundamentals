/*
 * X86_64 AT&T implementations of drawing functions and Helper functions
 * CSF Assignment 2
 * Bob Dong / Aidan Alme
 * bdong9@jh.edu / aalme2@jhu.edu
*/
	
/* Offsets of struct Image fields */
#define IMAGE_WIDTH_OFFSET   0
#define IMAGE_HEIGHT_OFFSET  4
#define IMAGE_DATA_OFFSET    8

/* Offsets of struct Rect fields */
#define RECT_X_OFFSET        0
#define RECT_Y_OFFSET        4
#define RECT_WIDTH_OFFSET    8
#define RECT_HEIGHT_OFFSET   12

	.section .text

/***********************************************************************
   Helper functions
   You should declare these as .globl so that
   they can be called from the unit test code.
 ***********************************************************************/

/*
 * Check if a pair of x and y coordinates are in bounds of a given image
 *
 * Parameters:                                                                                                                                                                                                 
 *   %rdi     - pointer to struct Image                                                                                                                                                                        
 *   %esi     - x coordinate (pixel column)                                                                                                                                                                    
 *   %edx     - y coordinate (pixel row)
 *
 * Return:
 *   %eax     - 0 if false, 1 if true
 */
	.globl in_bounds
in_bounds:
	// Check that x and y are not less than zero
	cmp $0, %esi
	jl .Lout_of_bounds
	cmp $0, %edx
	jl .Lout_of_bounds
	// Unsigned comparison to verify x
	cmpl IMAGE_WIDTH_OFFSET(%rdi), %esi 
	// Return 0 if x is x >= width
	jge .Lout_of_bounds
	// Continue if x < width
	// Unsigned comparison to verify y
	cmpl IMAGE_HEIGHT_OFFSET(%rdi), %edx 
	// Return 0 if y >= height
        jge .Lout_of_bounds
	// Continue if y < height
	// Return 1 because in_bounds == true
	movl $1, %eax  
	ret
.Lout_of_bounds:
	// Return 0 because in_bounds == false
        movl $0, %eax
        ret

	
/* 
 * Checks if tile is within the specified image. 
 * 
 * Parameters:
 *   %rdi - pointer to the tilemap
 *   %rsi - pointer to the tile
 *
 * Returns:
 *   a boolean statement indicating whether x and y are in bounds or not. 
 *
*/

	.globl check_valid_tile
check_valid_tile:
	// Align the stack                                                                                                                                                                                     
        push %rbp
        mov %rsp, %rbp
        subq $8, %rsp
	// Initialize %r13d to -1; it will be i for the for loops                                                                                                                                              
        movl $-1, %r13d
	// Start the outer loop
	jmp .Louter_loop2

.Louter_loop2:
	// Increment i by 1
	inc %r13d
	// Check i < tile->width
	cmp RECT_WIDTH_OFFSET(%rsi), %r13d
	// If the outer loop ends, valid_tile == true
	jg .Lret_true
	// Initialize %r14d to -1; it will be j for the for loops
	movl $-1, %r14d
        // Continue to the inner loop                                                                                                                                                                          
        jmp .Linner_loop2
	
.Linner_loop2:
	// Increment j by 1                                                                                                                                                                                    
        inc %r14d
	// Check j < tile->height                                                                                                                                                                       
        cmp RECT_HEIGHT_OFFSET(%rsi), %r14d
        // If the inner loop ends, go to the next iteration of the outer loop
        jg .Louter_loop2
	// Store the values for the necessary registers
	pushq %rdx
	pushq %r8
	// Move the values for in_bounds
	movl RECT_X_OFFSET(%rsi), %r8d
	movl RECT_Y_OFFSET(%rsi), %edx
	pushq %rsi
	movl %r8d, %esi
	// Add the displacement
	addl %r13d, %esi
	addl %r14d, %edx
	// Check that coordinates are in bounds
	call in_bounds
	// Restore the registers
	popq %rsi
	popq %r8
	popq %rdx
	// Return false if not in bounds
	cmp $0, %rax
	je .Lret_false
	// Else go to the next iteration of the inner loop
	jmp .Linner_loop2
	
.Lret_true:
	// Return true == 1
	movq $1, %rax
	// Fix the stack
	add $8, %rsp
        pop %rbp
	ret

.Lret_false:
	// Return false == 0
	movq $0, %rax
	// Fix the stack
	add $8, %rsp
        pop %rbp
	ret
	
/*                                                                                                                                                                                                             
 * Compute the array index representation of a give set of x y coordinates                                                                                                                                     
 *                                                                                                                                                                                                             
 * Parameters:                                                                                                                                                                                                 
 *   %rdi     - pointer to struct Image                                                                                                                                                                        
 *   %esi     - x coordinate (pixel column)                                                                                                                                                                    
 *   %edx     - y coordinate (pixel row)
 *
 * Return:                                                                                                                                                                                                     
 *   %eax     - the index                                                                                                                                                                          
 */
	.globl compute_index
compute_index:
	// Copy the y value into the return register
	movl %edx, %eax
	// Store %rdx                                                                                                                                                                                  
        pushq %rdx
        // Initialize %rdx                                                                                                                                                                                     
        movq $0, %rdx
	// Multiply the y value by the number of columns
	imull IMAGE_WIDTH_OFFSET(%rdi)
	// Return %rdx
	popq %rdx
	// Add the x value
	addl %esi, %eax
	ret

	
/*
* Clamps a value between a minimum and maximum range.
 *
 * Parameters:
 * %edi - the value to be clamped
 * %esi - the minimum value of the range
 * %edx - the maximum value of the range
 *
 * Returns:
* %eax - the clamped value, ensuring it is within the specified range
 */
	.globl clamp
clamp:
	//compare min with val
	cmp %esi, %edi
	//jump to min if val is less than min
	jle .Lmin
	//compare val with max
	cmp %edx, %edi
	//jump to max if val is greater than max
	jge .Lmax
	//if nothing happens, move val to return register
	mov %edi, %eax
	//return
	ret
.Lmin:
	//move return min to return rigiter
	mov %esi, %eax
	//return
	ret

.Lmax:
	//move max to return register
	mov %edx, %eax
	//return
	ret
	
/*
 * Get the 1 byte value for the red portion of the color
 *
 * Parameter:
 *     %edi - uint32_t color value
 *
 * Return:
 *     %eax - the one byte representation of the red color
 */
	.globl get_r
get_r:
	//load parameter into eax
	mov %edi, %eax
	//bit shift right by 24
	shr $24, %eax
	//return
	ret

/*                                                                                                                                                                                                             
 * Get the 1 byte value for the green portion of the color                                                                                                                                                     
 *                                                                                                                                                                                                             
 * Parameter:                                                                                                                                                                                                  
 *     %edx - uint32_t color value                                                                                                                                                                             
 *                                                                                                                                                                                                             
 * Return:                                                                                                                                                                                                     
 *     %edi - the one byte representation of the green color                                                                                                                                                  
 */
        .globl get_g
get_g:
	//load parameter into eax
	mov %edi, %eax
	//bit shift left 8
	shll $8, %eax
	//bit shit right 24
        shr $24, %eax
	//return 
        ret

/*                                                                                                                                                                                                             
 * Get the 1 byte value for the blue portion of the color                                                                                                                                                      
 *                                                                                                                                                                                                             
 * Parameter:                                                                                                                                                                                                  
 *     %edi - uint32_t color value                                                                                                                                                                             
 *                                                                                                                                                                                                             
 * Return:                                                                                                                                                                                                     
 *     %eax - the one byte representation of the blue color                                                                                                                                                    
 */
        .globl get_b
get_b:
	//load edi content into eax
	mov %edi, %eax
	//shift left 16 bits
        shll $16, %eax
	//shift right 24 bits
        shr $24, %eax
	//return 
        ret

/*                                                                                                                                                                                                             
 * Get the 1 byte value for the opacity portion of the color                                                                                                                                                   
 *                                                                                                                                                                                                             
 * Parameter:                                                                                                                                                                                                  
 *     %edi - uint32_t color value                                                                                                                                                                             
 *                                                                                                                                                                                                             
 * Return:                                                                                                                                                                                                     
 *     %edi - the one byte representation of the opacity                                                                                                                                                     
 */
        .globl get_a
get_a:
	//load edi into eax
	mov %edi, %eax
	//eax << 24
        shll $24, %eax
	//eax >> 24
        shr $24, %eax
	//return 
	ret

/*
 * Blends the color components of the foreground and background colors based on the alpha value.
 *
 * Parameters:
 * %edi - the foreground color in ARGB format
 * %esi - the background color in ARGB format
 * %edx - the alpha value for blending
 *
 * Returns:
 * %eax - the blended color in ARGB format
*/
    
    .global blend_components

blend_components:

	// Initialize %e12 to 0                                                                                                                                                                        
        movl $0, %r12d
	
	// Red portion
	// Copy the alpha value to %r9d
	movl %edx, %r9d
	// Store $255 in %r9d
	movl $255, %r10d
	// Subtract alpha from 255
	subl %r9d, %r10d
	// Store the foreground color
	pushq %rdi
	// Move the background color into %edi
	movl %esi, %edi
	// Get the value for the background in %eax
	call get_r
	// Restore the value of %edi
	popq %rdi
	// Store %rdx
	pushq %rdx
	// Initialize %rdx                                                                                                                                                                                     
        movq $0, %rdx
	// Multiply 255 - alpha by the background red
	mull %r10d
	// Restore %rdx
	popq %rdx
	// Move the product to %r10d
	movl %eax, %r10d
	// Copy the alpha value to %e11
	movl %edx, %r11d
	// Get the value for the foreground in %eax
	call get_r
	// Store %rdx                                                                                                                                                                                          
        pushq %rdx
	// Initialize %rdx
	movq $0, %rdx
	// Multiply the foreground red by alpha
	mull %r11d
	// Restore %rdx                                                                                                                                                                                        
        popq %rdx
	// Move the product to %r11d                                                                                                                                                                           
        movl %eax, %r11d
	// Add %e10 and %e11
	addl %r10d, %r11d
	// Move %e11 to %eax for division
	movl %r11d, %eax
	// Store the value of %edx
	pushq %rdx
	// Initialize %rdx                                                                                                                                                                                     
        movq $0, %rdx
	// Store 255 in %r8d
	movl $255, %r8d
	// Divide %eax by 255
	divl %r8d
	// Restore the value of %edx
	popq %rdx
	// Store the quotient in %e10
	movl %eax, %r10d
	// Store the value of %edi
	pushq %rdi
	// Store the value of %esi
	pushq %rsi
	// Store the value of %edx
	pushq %rdx
	// Store the value to be clamped, %e10, in %edi
	movl %r10d, %edi
	// Store the minimum in %esi
	movl $0, %esi
	// Store the maximum in %edx
	movl $255, %edx
	// Call clamp
	call clamp
	// Shift %eax, the clamped color value
	shll $24, %eax
	// Add the clamped color to $e12
	addl %eax, %r12d
	// Restore the value of %edx
	popq %rdx
	// Restore the value of %esi
	popq %rsi
	// Restore the value of %edi
	popq %rdi                                                                                                                                                                             

	
	// Green portion
	// Copy the alpha value to %r9d
	movl %edx, %r9d
	// Store $255 in %r9d
	movl $255, %r10d
	// Subtract alpha from 255
	subl %r9d, %r10d
	// Store the foreground color
	pushq %rdi
	// Move the background color into %edi
	movl %esi, %edi
	// Get the value for the background in %eax
	call get_g
	// Restore the value of %edi
	popq %rdi
	// Store %rdx
	pushq %rdx
	// Initialize %rdx                                                                                                                                                                                     
        movq $0, %rdx
	// Multiply 255 - alpha by the background red
	mull %r10d
	// Restore %rdx
	popq %rdx
	// Move the product to %r10d
	movl %eax, %r10d
	// Copy the alpha value to %e11
	movl %edx, %r11d
	// Get the value for the foreground in %eax
	call get_g
	// Store %rdx                                                                                                                                                                                          
        pushq %rdx
	// Initialize %rdx                                                                                                                                                                                     
        movq $0, %rdx
	// Multiply the foreground red by alpha
	mull %r11d
	// Restore %rdx                                                                                                                                                                                        
        popq %rdx
	// Move the product to %r11d                                                                                                                                                                           
        movl %eax, %r11d
	// Add %e10 and %e11
	addl %r10d, %r11d
	// Move %e11 to %eax for division
	movl %r11d, %eax
	// Store the value of %edx
	pushq %rdx
	// Initialize %rdx                                                                                                                                                                                     
        movq $0, %rdx
	// Store 255 in %r8d
	movl $255, %r8d
	// Divide %eax by 255
	divl %r8d
	// Restore the value of %edx
	popq %rdx
	// Store the quotient in %e10
	movl %eax, %r10d
	// Store the value of %edi
	pushq %rdi
	// Store the value of %esi
	pushq %rsi
	// Store the value of %edx
	pushq %rdx
	// Store the value to be clamped, %e10, in %edi
	movl %r10d, %edi
	// Store the minimum in %esi
	movl $0, %esi
	// Store the maximum in %edx
	movl $255, %edx
	// Call clamp
	call clamp
	// Shift %eax, the clamped color value
	shll $16, %eax
	// Add the clamped color to $e12
	addl %eax, %r12d
	// Restore the value of %edx
	popq %rdx
	// Restore the value of %esi
	popq %rsi
	// Restore the value of %edi
	popq %rdi                                                                                                                                                                             

	
	// Blue portion
	// Copy the alpha value to %r9d
	movl %edx, %r9d
	// Store $255 in %r9d
	movl $255, %r10d
	// Subtract alpha from 255
	subl %r9d, %r10d
	// Store the foreground color
	pushq %rdi
	// Move the background color into %edi
	movl %esi, %edi
	// Get the value for the background in %eax
	call get_b
	// Restore the value of %edi
	popq %rdi
	// Store %rdx
	pushq %rdx
	// Initialize %rdx                                                                                                                                                                                     
        movq $0, %rdx
	// Multiply 255 - alpha by the background red
	mull %r10d
	// Restore %rdx
	popq %rdx
	// Move the product to %r10d
	movl %eax, %r10d
	// Copy the alpha value to %e11
	movl %edx, %r11d
	// Get the value for the foreground in %eax
	call get_b
	// Store %rdx                                                                                                                                                                                          
        pushq %rdx
	// Initialize %rdx                                                                                                                                                                                     
        movq $0, %rdx
	// Multiply the foreground red by alpha
	mull %r11d
	// Restore %rdx                                                                                                                                                                                        
        popq %rdx
	// Move the product to %r11d                                                                                                                                                                           
        movl %eax, %r11d
	// Add %e10 and %e11
	addl %r10d, %r11d
	// Move %e11 to %eax for division
	movl %r11d, %eax
	// Store the value of %edx
	pushq %rdx
	// Initialize %rdx                                                                                                                                                                                     
        movq $0, %rdx
	// Store 255 in %r8d
	movl $255, %r8d
	// Divide %eax by 255
	divl %r8d
	// Restore the value of %edx
	popq %rdx
	// Store the quotient in %e10
	movl %eax, %r10d
	// Store the value of %edi
	pushq %rdi
	// Store the value of %esi
	pushq %rsi
	// Store the value of %edx
	pushq %rdx
	// Store the value to be clamped, %e10, in %edi
	movl %r10d, %edi
	// Store the minimum in %esi
	movl $0, %esi
	// Store the maximum in %edx
	movl $255, %edx
	// Call clamp
	call clamp
	// Shift %eax, the clamped color value
	shll $8, %eax
	// Add the clamped color to $e12
	addl %eax, %r12d
	// Restore the value of %edx
	popq %rdx
	// Restore the value of %esi
	popq %rsi
	// Restore the value of %edi
	popq %rdi                                                                                                                                                                             
	
	// Add 255 to the color
	addl $255, %r12d
	// Move %r12d to the return register
	movl %r12d, %eax
	
	ret
    
/*
 * Blend two colors with the opacity of the foreground
 *
 * Return:
 *     The blended color
 */
	.globl blend_colors
blend_colors:
	// Get the foreground opacity value
	call get_a
	// Store the return value from get_a in %ebx
	movl %eax, %edx
	// Blend the colors
	call blend_components
	ret

/*                                                                                                                                                                                                             
 * Set one pixel in an image with blending.                                                                                                                                                                    
 *                                                                                                                                                                                                             
 * Parameters:                                                                                                                                                                                                 
 *   %edi     - pointer to struct Image                                                                                                                                                                       
 *   %esi     - index
 *   %edx     - color
*/

	.globl set_pixel
set_pixel:

	//set up stack frame
	push %rbp
	mov %rsp, %rbp
	subq $8, %rsp

	//move parameters into eax and ecx
	mov %rdi, %rax
	mov %esi, %ecx

	//calculate the offset
	mov IMAGE_DATA_OFFSET(%rax), %r8

	//dereference data
	lea (%r8, %rcx, 4), %rcx

	//move the dereferenced daa into esi for second parameter
	mov (%rcx), %esi

	//move the color into first param
	mov %edx, %edi

	//call function and store the returned content to rcx
	call blend_colors
	mov %eax, (%rcx)

	//eradicate stack frame
	add $8, %rsp
	pop %rbp
	ret	
	

/*
 * Calculate the square of an integer
 *
 * Parameters:
 * 	%rdi - the integer to be squared
 *
 * Return:
 *	%rax - the square
 */
        .globl square
square:
	// Align the stack
	subq $8, %rsp
	// Store %rdx
	pushq %rdx
	// Move the integer to %rax for multiplication
	movl %edi, %eax
	// Multiply %rax by %rsi and store the product in %rax
	imull %edi
	// Restore %rdx
	popq %rdx
	// Unalign the stack
	addq $8, %rsp
        ret

/*
 * Calculate the square distance between two points
 *
 * Parameters:
 * 	%rdi - x1
 * 	%rsi - y1
 * 	%rdx - x2
 * 	%rcx - y2
 *
 * Return
 *	 %eax - the square distance
 */
        .globl square_dist
square_dist:
	// Align the stack                                                                                                                                                                                     
        subq $8, %rsp

	// Calculate the first square
	// Store the original %rdi
	pushq %rdi
	// Subtract x2 from x1
	subq %rdx, %rdi
	// Square the difference
	call square
	// Store the original %rdx
	pushq %rdx
	// Store the difference squared in %rdx
	movq %rax, %rdx

	// Calculate the second square
	// Move y1 to %rdi
	movq %rsi, %rdi
	// Subtract y2 from y1
	subq %rcx, %rdi
	// Square the difference
	call square

	// Add the squared differences
	addq %rdx, %rax
	// Restore %rdx
	popq %rdx
	// Restore %rdi
	popq %rdi
	// Unalign the stack                                                                                                                                                                                   
        addq $8, %rsp
        ret
	
/***********************************************************************
   Public API functions
 ***********************************************************************/

/*
 * Draw a pixel.
 *
 * Parameters:
 *   %edi     - pointer to struct Image
 *   %esi     - x coordinate (pixel column)
 *   %edx     - y coordinate (pixel row)
 *   %ecx     - uint32_t color value
 */
	.globl draw_pixel
draw_pixel:
	// Initialize the return register
	mov $0, %eax
	// Check if the coordinates are in bound
	call in_bounds
	// Return if in_bounds returns 0 == false
	cmp $0, %eax
	je .Lout
	// Continue if in_bounds returns 1 == true
	// Compute the index 
	call compute_index
	// Move the index value to %esi
	movl %eax, %esi
	// Move the ecx into edx
	movl %ecx, %edx
	// Call set_pixel to draw the pixel
	call set_pixel
	ret
.Lout:
	// Exit the function
	ret


/*
 * Draw a rectangle.
 * The rectangle has rect->x,rect->y as its upper left corner,
 * is rect->width pixels wide, and rect->height pixels high.
 *
 * Parameters:
 *   %rdi     - pointer to struct Image
 *   %rsi     - pointer to struct Rect
 *   %edx     - uint32_t color value
 */

	
	.globl draw_rect
draw_rect:
	// Initialize %r15
	movq $0, %r15
	// Initialize %r13d to -1; it will be i for the for loops
	movq $-1, %r13
	// Align the stack
	push %rbp
        mov %rsp, %rbp
	subq $8, %rsp

.Louter_loop4:
	// Outer for loop for x axis
	// Increment i by 1
	inc %r13d
	// Check that i < rect->width
	cmp RECT_WIDTH_OFFSET(%rsi), %r13d
	// Terminate the outer loop and exit if i > r
	jge .Ldone4
	//Otherwise continue
	// Initialize %r14d to -1; it will be j for the for loops                                                                                                                                       
        movq $-1, %r14                                                                                                                                                                      
	// Continue to the inner loop
	jmp .Linner_loop4
	
.Linner_loop4:
	// Inner loop for y axis
	// Increment j by 1
	inc %r14d
	// Check that j < rect->height
	cmp RECT_HEIGHT_OFFSET(%rsi), %r14d
	// Terminate the inner loop if j > r, move to the next iteration of the outer loop
	jge .Louter_loop4
	// Otherwise continue
	// Store %rdi, %rsi, %rdx
	pushq %rdi
	pushq %rsi
	pushq %rdx
	pushq %rcx
	// Move registers for draw_pixel call
	movq %rdx, %rcx
	movq RECT_Y_OFFSET(%rsi), %rdx
	movq RECT_X_OFFSET(%rsi), %rsi
	// Calculate x and y offset
	addq %r13, %rsi
	addq %r14, %rdx
	// Call draw_pixel
	call draw_pixel
	// Restore registers                                                                                                                                                                           
	popq %rcx
	popq %rdx
        popq %rsi
        popq %rdi
	// Move to the next iteration of the inner loop
	jmp .Linner_loop4

.Ldone4:
	add $8, %rsp
	pop %rbp
	ret	
	
/*
 * Draw a circle.
 * The circle has x,y as its center and has r as its radius.
 *
 * Parameters:
 *   %rdi     - pointer to struct Image
 *   %esi     - x coordinate of circle's center
 *   %edx     - y coordinate of circle's center
 *   %ecx     - radius of circle
 *   %r8d     - uint32_t color value
 */
	.globl draw_circle
draw_circle:
	// Initialize %r15
	movq $0, %r15
	// Initialize %r13d to 0; it will be i for the for loops
	movq $0, %r13
	// Set i to -r - 1
	subl %ecx, %r13d
	dec %r13d
	// Align the stack
	push %rbp
        mov %rsp, %rbp
	subq $8, %rsp

.Louter_loop:
	// Outer for loop for x axis
	// Increment i by 1
	inc %r13d
	// Check that i <= r
	cmp %ecx, %r13d
	// Terminate the outer loop and exit if i > r
	jg .Ldone
	//Otherwise continue
	// Initialize %r14d to 0; it will be j for the for loops                                                                                                                                       
        movq $0, %r14
	// Set j to -r - 1                                                                                                                                                                                     
        subl %ecx, %r14d
	dec %r14d
	// Continue to the inner loop
	jmp .Linner_loop
	
.Linner_loop:
	// Inner loop for y axis
	// Increment j by 1
	inc %r14d
	// Check that j <= r
	cmp %ecx, %r14d
	// Terminate the inner loop if j > r, move to the next iteration of the outer loop
	jg .Louter_loop
	// Otherwise continue
	// Store %rdi, %rsi, %rdx, and %rcx
	pushq %rdi
	pushq %rsi
	pushq %rdx
	pushq %rcx
	pushq %r8
	// Move values for square_dist
	movl %esi, %edi
	movl %edx, %esi
	movl %ecx, %r8d
	movl %r13d, %edx
	movl %r14d, %ecx
	// Calculate x2 and y2
	addl %edi, %edx
	addl %esi, %ecx
	// Calculate the square distance
	call square_dist
	// Move the square distance to %r15
	movq %rax, %r15
	// Move the radius to %rdi
	movl %r8d, %edi
	// Calculate the square of the radius
	call square
	// Restore registers
	popq %r8
	popq %rcx
	popq %rdx
	popq %rsi
	popq %rdi
	// Check that the i, j coordinates are within the circle
	cmp %eax, %r15d
	// Jump to the next iteration of the inner loop if not in the circle
	ja .Linner_loop
	// Otherwise continue
	// Store the registers and initialize to zero
	pushq %rdi
        pushq %rsi
        pushq %rdx
        pushq %rcx
        pushq %r8
	// Move values for draw_pixel
        movl %r8d, %ecx
	// Calculate the coordinates
	addl %r13d, %esi
	addl %r14d, %edx
	// Call draw_pixel
	call draw_pixel
	// Restore registers                                                                                                                                                                           
        popq %r8
        popq %rcx
        popq %rdx
        popq %rsi
        popq %rdi
	// Move to the next iteration of the inner loop
	jmp .Linner_loop

.Ldone:
	add $8, %rsp
	pop %rbp
	ret

/*
 * Draw a tile by copying all pixels in the region
 * enclosed by the tile parameter in the tilemap image
 * to the specified x/y coordinates of the destination image.
 * No blending of the tile pixel colors with the background
 * colors should be done.
 *
 * Parameters:
 *   %rdi - pointer to Image (dest image)
 *   %esi - x coordinate of location where tile should be copied
 *   %edx - y coordinate of location where tile should be copied
 *   %rcx - pointer to Image (the tilemap)
 *   %r8  - pointer to Rect (the tile)
 */
	.globl draw_tile
draw_tile:
	// Align the stack                                                                                                                                                                             
        push %rbp
        mov %rsp, %rbp
        subq $8, %rsp
	// Store the registers
	pushq %rdi
	pushq %rsi
	pushq %rdx
	pushq %rcx
	pushq %r8
	// Move the data around
	movq %rcx, %rdi
	movq %r8, %rsi
	// Check valid tile
	call check_valid_tile
	// Restore the registers
	popq %r8
	popq %rcx
	popq %rdx
	popq %rsi
	popq %rdi
	// Exit if not a valid tile
	cmp $0, %rax
	je .Lexit5
	// Else initialize the outer loop
	movl $-1, %r13d
	jmp .Louter_loop5

.Louter_loop5:
	// Increment i
	inc %r13d
	// Check i < sprite->width
	cmp RECT_WIDTH_OFFSET(%r8), %r13d
	// Exit if >=
	jge .Lexit5
	// Else initialize the inner loop
	movl $-1, %r14d
        jmp .Linner_loop5
	
.Linner_loop5:
        // Increment j                                                                                                                                                                                         
        inc %r14d
        // Check i < sprite->width                                                                                                                                                                             
        cmp RECT_HEIGHT_OFFSET(%r8), %r14d
        // Exit to outer loop if >=                                                                                                                                                                            
        jge .Louter_loop5
	// Else continue and check if in bounds
	// Store the registers
        pushq %rsi
        pushq %rdx
	// Increment x and y
	addl %r13d, %esi
	addl %r14d, %edx
	// Check if in bounds
	call in_bounds
	// Restore the registers
	popq %rdx
	popq %rsi
	// Go to next inner loop if out of bounds
	cmp $0, %rax
	je .Linner_loop5
	// Store the registers
        pushq %rsi
        pushq %rdx
        pushq %rcx
        pushq %r8
	pushq %rdi
	// Offset x and y
	addl %r13d, %esi
	addl %r14d, %edx
	// Compute and store the image index
	call compute_index
	movl %eax, %r9d
	// Compute the tile index
	movq %rcx, %rdi
	movl RECT_X_OFFSET(%r8), %esi
	movl RECT_Y_OFFSET(%r8), %edx
	addl %r13d, %esi
        addl %r14d, %edx
	call compute_index
	movl %eax, %r10d
	// Get the color from the tile
	mov IMAGE_DATA_OFFSET(%rdi), %r8
	lea (%r8, %r10, 4), %rcx
	mov (%rcx), %r10d
	// Store the tile color in the target image 
	popq %rdi
	mov IMAGE_DATA_OFFSET(%rdi), %r8
	lea (%r8, %r9, 4), %r9
	movl %r10d, (%r9)
	// Restore the registers
	popq %r8
        popq %rcx
        popq %rdx
        popq %rsi
	// Continue the inner loops
	jmp .Linner_loop5

.Lexit5:
	// Fix the stack
        add $8, %rsp
        pop %rbp
	ret


/*
 * Draw a sprite by copying all pixels in the region
 * enclosed by the sprite parameter in the spritemap image
 * to the specified x/y coordinates of the destination image.
 * The alpha values of the sprite pixels should be used to
 * blend the sprite pixel colors with the background
 * pixel colors.
 *
 * Parameters:
 *   %rdi - pointer to Image (dest image)
 *   %esi - x coordinate of location where sprite should be copied
 *   %edx - y coordinate of location where sprite should be copied
 *   %rcx - pointer to Image (the spritemap)
 *   %r8  - pointer to Rect (the sprite)
 */
	.globl draw_sprite
draw_sprite:
	// Align the stack                                                                                                                                                                             
        push %rbp
        mov %rsp, %rbp
        subq $8, %rsp
	// Store the registers
	pushq %rdi
	pushq %rsi
	pushq %rdx
	pushq %rcx
	pushq %r8
	// Move the data around
	movq %rcx, %rdi
	movq %r8, %rsi
	// Check valid tile
	call check_valid_tile
	// Restore the registers
	popq %r8
	popq %rcx
	popq %rdx
	popq %rsi
	popq %rdi
	// Exit if not a valid tile
	cmp $0, %rax
	je .Lexit
	// Else initialize the outer loop
	movl $-1, %r13d
	jmp .Louter_loop3

.Louter_loop3:
	// Increment i
	inc %r13d
	// Check i < sprite->width
	cmp RECT_WIDTH_OFFSET(%r8), %r13d
	// Exit if >=
	jge .Lexit
	// Else initialize the inner loop
	movl $-1, %r14d
        jmp .Linner_loop3
	
.Linner_loop3:
        // Increment j                                                                                                                                                                                         
        inc %r14d
        // Check i < sprite->width                                                                                                                                                                             
        cmp RECT_HEIGHT_OFFSET(%r8), %r14d
        // Exit to outer loop if >=                                                                                                                                                                            
        jge .Louter_loop3
	// Else continue and check if in bounds
	// Store the registers
        pushq %rsi
        pushq %rdx
	// Increment x and y
	addl %r13d, %esi
	addl %r14d, %edx
	// Check if in bounds
	call in_bounds
	// Restore the registers
	popq %rdx
	popq %rsi
	// Go to next inner loop if out of bounds
	cmp $0, %rax
	je .Linner_loop3
	// Store the registers
        pushq %rsi
        pushq %rdx
        pushq %rcx
        pushq %r8
	pushq %rdi
	// Offset x and y
	addl %r13d, %esi
	addl %r14d, %edx
	// Compute and store the image index
	call compute_index
	movl %eax, %r9d
	// Compute the sprite index
	movq %rcx, %rdi
	movl RECT_X_OFFSET(%r8), %esi
	movl RECT_Y_OFFSET(%r8), %edx
	addl %r13d, %esi
        addl %r14d, %edx
	call compute_index
	movl %eax, %r10d
	// Get the color from the sprite
	mov IMAGE_DATA_OFFSET(%rdi), %r8
	lea (%r8, %r10, 4), %rcx
	mov (%rcx), %r10d
	// Move for set_pixel
	popq %rdi
	pushq %rdi
	movl %r9d, %esi
	movl %r10d, %edx
	// Set the pixel
	call set_pixel
	// Restore the registers
	popq %rdi
	popq %r8
        popq %rcx
        popq %rdx
        popq %rsi
	// Continue the inner loops
	jmp .Linner_loop3

.Lexit:
	// Fix the stack
        add $8, %rsp
        pop %rbp
	ret

/*
vim:ft=gas:
*/
