import unittest
from calc import emissionPoint

class TestFunctions(unittest.TestCase):

    def test_fact(self):
        example = {"x1": 5, "y1": -3, "z1": -8, "t1": 2, "x2": -10, "y2": 6, "z2": 16, "t2": 4}
        pos = emissionPoint(example)
        self.assertEqual(pos['x'], 0)
        self.assertEqual(pos['y'], 0)
        self.assertEqual(pos['z'], 0)


if __name__ == '__main__':
    unittest.main()