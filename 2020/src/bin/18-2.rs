use aoc_2020::benchmarked_main;

#[derive(Debug)]
enum Expression {
    Value(usize),
    Add(Box<(Expression, Expression)>),
    Multiply(Box<(Expression, Expression)>),
}

impl Expression {
    fn evaluate(&self) -> usize {
        match self {
            Expression::Value(value) => *value,
            Expression::Add(args) => {
                let (left, right) = args.as_ref();
                left.evaluate() + right.evaluate()
            },
            Expression::Multiply(args) => {
                let (left, right) = args.as_ref();
                left.evaluate() * right.evaluate()
            },
        }
    }
}

#[derive(Debug)]
enum ExpressionBuilderState {
    Initial,
    LeftArgEx,
    OpPre,
    Op,
    OpPost,
    SecondArg,
    SecondArgEx,
    SecondArgExChain,
    CompleteOrChain,
    ChainOp,
}

#[derive(Debug)]
enum Op {
    Add,
    Multiply,
}

impl From<char> for Op {
    fn from(ch: char) -> Self {
        match ch {
            '+' => Op::Add,
            '*' => Op::Multiply,
            _ => panic!("expected '+' or '*', not {}", ch),
        }
    }
}

#[derive(Debug)]
struct ExpressionBuilder {
    state: ExpressionBuilderState,
    left: Option<Expression>,
    op: Op,
    right: Option<Expression>,
    builder: Option<Box<ExpressionBuilder>>,
}

impl ExpressionBuilder {
    fn build(&mut self) -> Expression {
        match self.state {
            ExpressionBuilderState::OpPre => self.left.take().unwrap(),
            ExpressionBuilderState::SecondArgEx => {
                let args = Box::new((
                    self.left.take().unwrap(),
                    self.builder.take().unwrap().build(),
                ));
                match self.op {
                    Op::Add => Expression::Add(args),
                    Op::Multiply => Expression::Multiply(args),
                }
            },
            ExpressionBuilderState::CompleteOrChain
            | ExpressionBuilderState::ChainOp
            | ExpressionBuilderState::SecondArgExChain => {
                let args = Box::new((
                    self.left.take().unwrap(),
                    self.right.take().unwrap_or_else(|| {
                        self.builder.take().unwrap().build()
                    }),
                ));
                match self.op {
                    Op::Add => Expression::Add(args),
                    Op::Multiply => Expression::Multiply(args),
                }
            },
            _ => panic!("expression incomplete ({:?})", self),
        }
    }

    fn input(
        &mut self,
        ch: char,
    ) -> Option<Expression> {
        match self.state {
            ExpressionBuilderState::Initial => {
                if ch == '(' {
                    self.builder = Some(Box::new(ExpressionBuilder::default()));
                    self.state = ExpressionBuilderState::LeftArgEx;
                } else {
                    self.left = Some(Expression::Value(
                        ch.to_digit(10).unwrap() as usize,
                    ));
                    self.state = ExpressionBuilderState::OpPre;
                }
                None
            },
            ExpressionBuilderState::LeftArgEx => {
                let mut builder = self.builder.take().unwrap();
                if let Some(left) = builder.input(ch) {
                    self.left = Some(left);
                    self.state = ExpressionBuilderState::OpPre;
                } else {
                    self.builder.replace(builder);
                }
                None
            },
            ExpressionBuilderState::OpPre => {
                assert_eq!(ch, ' ');
                self.state = ExpressionBuilderState::Op;
                None
            },
            ExpressionBuilderState::Op => {
                self.op = Op::from(ch);
                self.state = ExpressionBuilderState::OpPost;
                None
            },
            ExpressionBuilderState::OpPost => {
                assert_eq!(ch, ' ');
                self.state = ExpressionBuilderState::SecondArg;
                None
            },
            ExpressionBuilderState::SecondArg => {
                if ch == '(' {
                    self.builder = Some(Box::new(ExpressionBuilder::default()));
                    self.state = ExpressionBuilderState::SecondArgEx;
                } else {
                    self.right = Some(Expression::Value(
                        ch.to_digit(10).unwrap() as usize,
                    ));
                    self.state = ExpressionBuilderState::CompleteOrChain;
                }
                None
            },
            ExpressionBuilderState::SecondArgEx => {
                let mut builder = self.builder.take().unwrap();
                if let Some(right) = builder.input(ch) {
                    self.right = Some(right);
                    self.state = ExpressionBuilderState::CompleteOrChain;
                } else {
                    self.builder.replace(builder);
                }
                None
            },
            ExpressionBuilderState::SecondArgExChain => {
                let mut builder = self.builder.take().unwrap();
                if let Some(right) = builder.input(ch) {
                    self.right = Some(right);
                    Some(self.build())
                } else {
                    self.builder.replace(builder);
                    None
                }
            },
            ExpressionBuilderState::CompleteOrChain => match ch {
                ' ' => {
                    self.state = ExpressionBuilderState::ChainOp;
                    None
                },
                ')' => Some(self.build()),
                ch => panic!("extra junk: {}", ch),
            },
            ExpressionBuilderState::ChainOp => {
                let op = Op::from(ch);
                match (&self.op, op) {
                    (Op::Multiply, Op::Add) => {
                        self.builder.replace(Box::new(ExpressionBuilder {
                            state: ExpressionBuilderState::OpPost,
                            left: self.right.take(),
                            op: Op::Add,
                            right: None,
                            builder: None,
                        }));
                        self.state = ExpressionBuilderState::SecondArgExChain;
                    },
                    (_, op) => {
                        self.left = Some(self.build());
                        self.op = op;
                        self.state = ExpressionBuilderState::OpPost;
                    },
                };
                None
            },
        }
    }
}

impl Default for ExpressionBuilder {
    fn default() -> Self {
        Self {
            state: ExpressionBuilderState::Initial,
            left: None,
            op: Op::Add,
            right: None,
            builder: None,
        }
    }
}

fn parse(input: &str) -> Vec<Expression> {
    input
        .lines()
        .map(|line| {
            let mut builder = ExpressionBuilder::default();
            for ch in line.trim().chars() {
                builder.input(ch);
            }
            builder.build()
        })
        .collect::<Vec<_>>()
}

fn solve_already_parsed<T: AsRef<[Expression]>>(input: &T) -> Option<usize> {
    Some(input.as_ref().iter().map(Expression::evaluate).sum())
}

const ITERATIONS: usize = 1000;

fn main() {
    benchmarked_main(parse, solve_already_parsed, ITERATIONS);
}
