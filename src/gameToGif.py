import chess
import chess.svg
import cairosvg
import PIL.Image
import PIL.ImageDraw
import PIL.ImageFont
import io

def parse_game_file(file_path):
    with open(file_path, 'r') as file:
        # Read player names
        white, black = file.readline().strip().split(',')
        # Read starting FEN
        starting_fen = file.readline().strip()
        # Read moves
        moves = file.readline().strip().split()
    
    return white, black, starting_fen, moves

def generate_board_images(starting_fen, moves, white, black):
    board = chess.Board(fen=starting_fen)
    images = []
    
    for move in moves:
        # Convert any promotion letter to lowercase
        if len(move) == 5:  # Promotion moves have 5 characters, e.g., "e7e8Q"
            move = move[:4] + move[4].lower()
        
        # Parse and apply the move
        uci_move = chess.Move.from_uci(move)
        if uci_move in board.legal_moves:
            board.push(uci_move)
        else:
            print(f"Illegal move encountered: {move}")
            break
        
        # Render the board as an SVG and then convert to an image
        svg_data = chess.svg.board(board)
        img = svg_to_image(svg_data)
        
        # Add player names to the image
        img = add_player_names(img, white, black)
        images.append(img)
    
    return images

def svg_to_image(svg_data):
    # Convert SVG data to PNG using cairosvg, then load as a PIL image
    png_data = cairosvg.svg2png(bytestring=svg_data.encode("utf-8"))
    img = PIL.Image.open(io.BytesIO(png_data))
    return img

def add_player_names(img, white, black):
    # Create a new image with space for names at the top
    new_img = PIL.Image.new("RGB", (img.width, img.height + 20), "white")
    new_img.paste(img, (0, 20))
    
    # Draw the player names
    draw = PIL.ImageDraw.Draw(new_img)
    font = PIL.ImageFont.load_default()  # You can use a custom font if preferred
    text = f"White: {white} vs Black: {black}"
    
    # Get the bounding box of the text for centering
    text_bbox = draw.textbbox((0, 10), text, font=font)
    text_width = text_bbox[2] - text_bbox[0]
    text_x = (new_img.width - text_width) // 2  # Center the text
    
    # Draw the text
    draw.text((text_x, 5), text, fill="black", font=font)
    
    return new_img

def save_gif(images, output_path, duration=500, end_duration=4000):
    # Extend the last frame duration by adding it multiple times
    extended_images = images + [images[-1]] * int(end_duration / duration)
    
    # Save images as a GIF
    extended_images[0].save(
        output_path,
        save_all=True,
        append_images=extended_images[1:],
        duration=duration,
        loop=0
    )

def create_chess_game_gif(file_path, output_path):
    # Parse the game file
    white, black, starting_fen, moves = parse_game_file(file_path)
    print(f"Creating GIF for game between {white} and {black}")
    
    # Generate images of board positions
    images = generate_board_images(starting_fen, moves, white, black)
    
    # Save images as a GIF
    save_gif(images, output_path)
    print(f"GIF saved to {output_path}")


def save_final_position(starting_fen, moves, white, black, output_path, size=2000):
    # Set up the final board position
    board = chess.Board(fen=starting_fen)
    
    # Play all moves to reach final position
    for move in moves:
        if len(move) == 5:
            move = move[:4] + move[4].lower()
        uci_move = chess.Move.from_uci(move)
        if uci_move in board.legal_moves:
            board.push(uci_move)
    
    # Generate high-resolution SVG
    svg_data = chess.svg.board(board, size=size)
    
    # Convert to high-quality PNG
    png_data = cairosvg.svg2png(bytestring=svg_data.encode("utf-8"), scale=1.0)
    img = PIL.Image.open(io.BytesIO(png_data))
    
    # Add player names with larger font
    new_img = PIL.Image.new("RGB", (img.width, img.height + 60), "white")
    new_img.paste(img, (0, 60))
    
    # Use larger font for better readability
    draw = PIL.ImageDraw.Draw(new_img)
    font_size = size // 40
    try:
        font = PIL.ImageFont.truetype("arial.ttf", font_size)
    except:
        font = PIL.ImageFont.load_default()
    
    text = f"White: {white} vs Black: {black}"
    text_bbox = draw.textbbox((0, 30), text, font=font)
    text_width = text_bbox[2] - text_bbox[0]
    text_x = (new_img.width - text_width) // 2
    
    draw.text((text_x, 10), text, fill="black", font=font)
    
    # Save the final image
    new_img.save(output_path, "PNG", quality=95, dpi=(300, 300))
    

file_path = "src/games/g.txt"
output_path = "src/games/g.gif"
final_image_path = "src/games/final_position.png"
create_chess_game_gif(file_path, output_path)
white, black, starting_fen, moves = parse_game_file(file_path)
save_final_position(starting_fen, moves, white, black, final_image_path)