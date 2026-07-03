# National Banking System

This is a Banking Management System I built for my OOP course — the core logic is written in C++, and I later added a full HTML/CSS/JS front-end on top of it to actually visualize how the whole thing works in a browser.

The C++ side handles everything through classes and file I/O (no database), while the front-end is basically a working mockup of the same system — registration, admin approvals, transactions, the works.

## What it can do

- **Registration** – Users and companies can sign up, but they sit in a pending queue until an admin approves them.
- **Admin panel** – Admin can approve or reject pending users/companies. Once a company gets approved, it's automatically issued a card.
- **Banking basics** – Deposits, withdrawals, transfers, and card issuance. Each account type (Standard, Gold, Business) has its own balance cap and daily transaction limit.
- **Fraud controls** – Accounts and cards can be frozen, and suspicious accounts can be flagged.
- **Transaction logs** – Every deposit, withdrawal, and transfer gets logged, so there's a record of everything.
- **Loans** – Basic loan request handling for accounts that qualify.
- **No database needed** – Everything is saved to plain `.txt` files, so the C++ program works standalone.

## Built with

| Part           | Tech                            |
| Backend logic  | C++ (OOP, file handling)        |
| Frontend demo  | HTML, CSS, vanilla JS           |

## Folder structure

├── BankingSystem.cpp   # The actual C++ program
├── index.html          # Browser-based demo/UI
└── README.md


## How to run it

## The C++ version

```bash
g++ -o BankingSystem BankingSystem.cpp
./BankingSystem
```

First time you run it, it'll generate its own `.txt` files (for users, cards, transactions, etc.) in the same folder — you don't need to set anything up beforehand.

## The front-end demo

No setup at all — just open `index.html` in your browser and you're good to go.

## A quick note

This was built purely as a learning project — mainly to get more comfortable with OOP concepts and file handling in C++, and to try designing a front-end around it. It's not meant for real-world/production banking use.

## License

MIT , see license for the details.