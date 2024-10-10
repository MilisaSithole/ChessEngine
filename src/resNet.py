import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
from torch.utils.data import DataLoader, Dataset
import chess
import numpy as np

class ResBlock(nn.Module):
    def __init__(self, numHidden):
        super().__init__()

        self.conv1 = nn.Conv2d(numHidden, numHidden, kernel_size = 3, padding = 1)
        self.bn1 = nn.BatchNorm2d(numHidden)
        self.conv2 = nn.Conv2d(numHidden, numHidden, kernel_size = 3, padding = 1)
        self.bn2 = nn.BatchNorm2d(numHidden)

    def forward(self, x):
        residual = x

        x = F.relu(self.bn1(self.conv1(x)))
        x = self.bn2(self.conv2(x))
        x += residual
        x = F.relu(x)

        return x
    
class ResNet(nn.Module):
    def __init__(self, numResBlocks, numInChannels, numHidden, numOut, device):
        super().__init__()

        self.device = device

        # Input layer
        self.startBlock = nn.Sequential(
            nn.Conv2d(numInChannels, numHidden, kernel_size = 3, padding = 1),
            nn.BatchNorm2d(numHidden),
            nn.ReLU()
        )

        # Residual blocks
        self.backBone = nn.ModuleList(
            [ResBlock(numHidden) for _ in range(numResBlocks)]
        )

        # Policy head
        self.policyHead = nn.Sequential(
            nn.Conv2d(numHidden, 32, kernel_size = 3, padding = 1),
            nn.BatchNorm2d(32),
            nn.ReLU(),
            nn.Flatten(),
            nn.Linear(32 * 8 * 8, numOut),
            nn.Softmax()
        )

        # Value head
        self.valueHead = nn.Sequential(
            nn.Conv2d(numHidden, 16, kernel_size = 3, padding = 1),
            nn.BatchNorm2d(16),
            nn.ReLU(),
            nn.Flatten(),
            nn.Linear(16 * 8 * 8, 1),
            nn.Tanh()
        )

        self.to(device)

    def forward(self, x):
        x = self.startBlock(x)

        for resBlock in self.backBone:
            x = resBlock(x)

        policy = self.policyHead(x)
        value = self.valueHead(x)
        return policy, value    

class ChessDataset(Dataset):
    def __init__(self, positions, policyLabels, valueLabels):
        """
        Initializes a ChessDataset object with the given positions, policy labels, and value labels.
        
        Args:
            positions (list): A list of positions (e.g. chess board states).
            policyLabels (list): A list of policy labels corresponding to the positions.
            valueLabels (list): A list of value labels corresponding to the positions.
        """
        self.positions = positions
        self.policyLabels = policyLabels
        self.valueLabels =  valueLabels

    def __len__(self):
        return len(self.positions)
    
    def __getitem__(self, idx):
        return self.positions[idx], self.policyLabels[idx], self.valueLabels[idx]
    

def boardToTensor(board: chess.Board):
    tensor = np.zeros((18, 8, 8), dtype = np.float32)

    # Add pieces to the tensor [12]
    pieceMap = board.piece_map()
    for square, piece in pieceMap.items():
        row, col = divmod(square, 8)
        channel = pieceIndex(piece)
        tensor[channel, row, col] = 1

    # Add player turn [1]
    if board.turn == chess.BLACK:
        tensor[12, :, :] = 1

    # Add castling rights [4]
    if board.has_kingside_castling_rights(chess.WHITE):
        tensor[13, :, :] = 1
    if board.has_queenside_castling_rights(chess.WHITE):
        tensor[14, :, :] = 1
    if board.has_kingside_castling_rights(chess.BLACK):
        tensor[15, :, :] = 1
    if board.has_queenside_castling_rights(chess.BLACK):
        tensor[16, :, :] = 1

    # Add en passant square [1]
    if board.has_legal_en_passant():
        row, col = divmod(board.ep_square, 8)
        tensor[17, row, col] = 1

    return tensor

def pieceIndex(piece: chess.Piece) -> int:
    """
    Returns the index of the given chess piece in the tensor.

    Args:
        piece (chess.Piece): The chess piece to get the index of.

    Returns:
        int: The index of the given chess piece in the tensor.
    """
    idx = 0
    if piece.piece_type == chess.PAWN:
        idx = 0
    elif piece.piece_type == chess.ROOK:
        idx = 1
    elif piece.piece_type == chess.KNIGHT:
        idx = 2
    elif piece.piece_type == chess.BISHOP:
        idx = 3
    elif piece.piece_type == chess.QUEEN:
        idx = 4
    elif piece.piece_type == chess.KING:
        idx = 5
    
    if piece.color == chess.BLACK:
        idx += 6

    return idx

def traceAndSave(model, input, title):
    model.eval()

    tracedModel = torch.jit.trace(model, input)

    tracedModel.save('src/files/' + title + '.pt')

# M A I N =============================================================
board = chess.Board("3r4/5pbk/P2P1n1p/q5p1/2Q2p2/5N1P/5PP1/4RBK1 w - - 1 31")
print(board)

# Init ResNet model
# model = ChessResNet([2, 2, 2, 2], 18, 1792)
device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
device = torch.device('cpu')
model = ResNet(4, 18, 64, 1968, device)
model.eval()

# Test input
inp = boardToTensor(board).reshape(-1, 18, 8, 8)
inp = torch.tensor(inp, device = device)

# Outputs
policy, value = model(inp)
policy = policy.squeeze(0).detach().cpu().numpy()
value = value.item()


# Save model
traceAndSave(model, inp, 'NewResNet')

print('\n======= O U T P U T S =======')
print(f'Policy:\t\t {policy}')
print(f'Policy Shape:\t {policy.shape}')
print(f'Policy sum:\t {np.sum(policy)}')
print(f'Best move:\t {np.argmax(policy)} with value {np.max(policy)}')
print(f'Value:\t\t {value}')
